#include <thrust/host_vector.h>
#include <thrust/device_vector.h>
#include <iostream>
#include <fstream>
#include <string>
#include "utils/des.h"

using namespace std;

#define IS_BIG_ENDIAN() (*(uint16_t *)"\0\xff"< 0x100)

//template structure to pass Thrust device vector to CUDA kernel
template <typename T>

struct KernelArray {
	T* _array;
	int _size;
};


//function to convert device vector to KernelArray structure
template <typename T>
KernelArray<T> convertToKernel(thrust::device_vector<T>& dVec) {
	KernelArray<T> kArray;
	kArray._array = thrust::raw_pointer_cast(&dVec[0]);
	kArray._size = (int) dVec.size();

	return kArray;
}



//function to convert a string to uint64
__device__ __host__ uint64_t str2uint64(const char* input){
	uint64_t output = 0;
	if (IS_BIG_ENDIAN()){
		for(int i = 0; i < 8; i++){
			output |= (uint64_t)input[i];
			if(i < 7)
				output <<= 8;
		}
	}
	else{
		for(int i = 7; i > -1; i--){
			output <<= 8;
			output |= (uint64_t)input[i];
		}
	}
	return output;
}




//kernel function
__global__ void decrypt_kernel(KernelArray<uint64_t> device_list, uint64_t u_salt, uint64_t crypt_pass, int *foundD, uint64_t *resultD) {

	uint64_t crypt;
	int i = threadIdx.x + blockIdx.x * blockDim.x;

	//stride is the total number of threads
	int stride = blockDim.x * gridDim.x;

	while (i < device_list._size) {
        //password encryption from the list of passwords
		crypt = full_des_encode_block(device_list._array[i], u_salt);

		if (crypt_pass == crypt) {
             //password found
			*foundD = 1;
			*resultD = crypt;
			printf("Password matched at position %d\n", i+1);
			return;
		}
		i += stride;
	}
}


int main(void) {
	//in host_list there is the storage of all the passwords in the list
	thrust::host_vector<uint64_t> host_list(1);
	int block_size = 128;
	string salt = "F4";
	string password_target;
	string passwords_to_find[] = {"Manuel96","Paololep","carlo666"};
	//string passwords_to_find[] = {"freese93","feelin74","RaZeRis6","pan20139","cork1084","38993414","ybrf1721","vatino04","marioro9", "carlo666"};


	int* foundD;  //used in the device to confirm that the password_target was found
	uint64_t* resultD; //used in the device to storage the result

	//memory allocation
	cudaMalloc((void**)&foundD, sizeof(int));
	cudaMalloc((void**)&resultD, sizeof(uint64_t));
	cudaMemset(foundD, 0, sizeof(int));
	cudaMemset(resultD, 0, sizeof(uint64_t));

	int* foundH;  //used in the host
	uint64_t* resultH;  //used in the host

	//memory allocation
	foundH = (int *)malloc(sizeof(int));
	resultH = (uint64_t *)malloc(sizeof(uint64_t));

	int index = 0; //index used to resize the host list
	ifstream inputfile("password_dictionaryNew.txt");

	if (inputfile.is_open()) {
		string password;
		while (getline(inputfile, password)) {
			host_list[index] = str2uint64(password.c_str());
			index++;
			host_list.resize(index+1);
		}

		host_list.resize(index);
		inputfile.close();

	}
	else
		cout << "Error open file!" << endl;

	//block and grid dimension
	dim3 blockDim(block_size);
	dim3 gridDim(host_list.size()/blockDim.x + 1);

	 for(int i = 0; i<3; i++){
		password_target=passwords_to_find[i];
		for(int j = 0; j<3; j++){
			//returns the size of vector host_list
			std::cout << "Password list has size:  " << host_list.size() << std::endl;

			

			uint64_t u_salt = str2uint64(salt.c_str());
			uint64_t pass = str2uint64(password_target.c_str());
			uint64_t crypt_pass;
			//password_target encryption
			crypt_pass = full_des_encode_block(pass,u_salt);
			cout << "Password to find:" << password_target << endl;
			cout << "Password (crypted) to find: " << crypt_pass << endl;

			//transfer data to the device
			thrust::device_vector<uint64_t> device_list = host_list;

			clock_t time_start = clock();

			//threads running on the GPU
			decrypt_kernel<<<gridDim, blockDim>>>(convertToKernel(device_list), u_salt, crypt_pass, foundD, resultD );


			//save found and result to the host memory
			cudaMemcpy(foundH, foundD, sizeof(int), cudaMemcpyDeviceToHost);
			cudaMemcpy(resultH, resultD, sizeof(uint64_t), cudaMemcpyDeviceToHost);


			if (*foundH == 1) {
				clock_t time_end = clock();
				float time_elapsed = (float)(time_end - time_start) / CLOCKS_PER_SEC;
				cout << "Password found!" << endl;
				cout << "Time elapsed in GPU: "<< time_elapsed * 1000 << " ms" << endl;
				cout << "The search result is : " << *resultH << endl << endl;
			}
			else
				cout << "Error! Password not found!" << endl;


			//data from device to host
			thrust::copy(device_list.begin(), device_list.end(), host_list.begin());

		}
	}
	return 0;
}



