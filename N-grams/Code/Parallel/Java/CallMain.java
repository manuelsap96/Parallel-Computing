import java.io.*;
import java.util.*;
import java.time.Instant;
import java.time.Duration;
import java.util.concurrent.*;


public class CallMain implements Callable<List<String>> {
    private int n; //the n of n-gram
    private String str;  //the entire text
    private int start; //size of chunks
    private int end;



    public CallMain(int n, String str, int start, int end) {
        this.n = n;
        this.str = str;
        this.start = start;
        this.end = end;
    }


    public List<String> call() throws Exception{
        //call function
        //it computes n-grams from a string that contains all words
        String letters = "abcdefghijklmnopqrstuvwxyz";
        boolean skip;
        List<String> ngrams = new ArrayList<>();
        assert start <= end;
        for (int i = start; i < end - n + 1; i++) {
            skip = false;
            for (int k = 0; k < n ; k++) {

                if (letters.indexOf(str.charAt(i+k)) == -1) {
                    skip = true;
                    break;
                }
            }
            if (skip) continue;
            ngrams.add(str.substring(i, i + n));
        }
        return ngrams;
    }


    public static String myReadFile(String filename) {
        //function that read the file and convert it in a text string
        //in the return it delete punctuation and covert the string to lower case
        try {
            BufferedReader reader = new BufferedReader(new FileReader(filename));
            StringBuilder output = new StringBuilder();
            String line;
            while ((line = reader.readLine()) != null) {
                output.append(line);
                output.append(" "); //used to distance the various rows of the file
            }
            reader.close();
            return output.toString().toLowerCase();

        } catch (IOException e) {
            System.err.format("Exception occurred trying to read '%s'.", filename);
            e.printStackTrace();
            return null;
        }
    }


    public static void myWriteFile(String fileName, List<String> text) {
        //function used to write the list of string content into a text file
        try {
            BufferedWriter writer = new BufferedWriter(new FileWriter(fileName));
            StringBuilder output = new StringBuilder();
            for (String word : text){
                writer.write(word + " ");
            }
            writer.close();
        } catch (IOException e) {
            System.err.format("Exception occurred trying to write '%s'.", fileName);
            e.printStackTrace();
        }
    }


    public static List<Integer> create_chunks(int numb_th, String str) {
        //function to create chunks of text for all the threads
        List<Integer> result = new ArrayList<>();
        int pos;
        result.add(0);
        int chunck = str.length() / numb_th;
        for (int k = 1; k < numb_th; k++ ) {
            pos = chunck * k;
          // if the cut of the string happens in the middle of a word, go ahead to an empty character
            while (str.charAt(pos) != ' ') {
                pos += 1;
            }
            result.add(pos);
        }
        result.add(str.length());
        return result;
    }


    public static LinkedHashMap<String, Float> frequency(List<String> list_ngrams) {
        //function that computes the frequence of each n-gram in the selected text
        LinkedHashMap<String, Float> freq_dict = new LinkedHashMap<>(); //elements sorted by insertion
        int tot;  //total number of an n -gram
        for (int i = 0 ; i < list_ngrams.size() ; i++) {
            //if the n-gram is already in freq_dict it goes to the next
            if (freq_dict.containsKey(list_ngrams.get(i))) {
                continue;
            }

            tot = 0;
            for (int k = i ; k < list_ngrams.size() ; k++) {
                if (list_ngrams.get(i).equals(list_ngrams.get(k))) {
                    tot++;
                }
            }
            freq_dict.put(list_ngrams.get(i), (float) tot / list_ngrams.size());
        }
        return freq_dict;
    }


    public static void main(String[] Args) throws InterruptedException {

        String book = myReadFile("orgoglio_32mb.txt");
        assert book != null;

        //number_th is the number of threads
        int number_th = 4;

        List<Integer> ck = create_chunks(number_th, book);

        List<String> ngrams = new ArrayList<>();
        int count = 0;

        //executors creation
        ThreadPoolExecutor executor = (ThreadPoolExecutor) Executors.newFixedThreadPool(4);
        List<Future<List<String>>> resultList = new ArrayList<>();

        Instant start_t = Instant.now();
        for (int i = 0; i < number_th; i++) {

            //threads creation
            CallMain prova = new CallMain(2, book, ck.get(i), ck.get(i+1));
            //exec threads and get back results
            Future<List<String>> result = executor.submit(prova);
            resultList.add(result);
        }

        for (Future<List<String>> future : resultList) {
            try
            {
                //ngrams.addAll(future.get()); //optional for the written file in output
                count = count + future.get().size();

            }
            catch (InterruptedException | ExecutionException e)
            {
                e.printStackTrace();
            }
        }
        executor.shutdown();
        Instant end_t = Instant.now();

        Duration timeElapsed = Duration.between(start_t, end_t);
        System.out.println("Time taken: "+ timeElapsed.toMillis() +" milliseconds");
        //LinkedHashMap<String, Float> freq = frequency(ngrams);
        //System.out.println(freq);
        //myWriteFile("ngrams_output.txt", ngrams);

    }


}

