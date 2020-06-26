import java.io.*;
import java.util.*;
import java.time.Instant;
import java.time.Duration;

public class Main {

    public static String myReadFile(String filename) {
        //function that read the file and convert it in a text string
        //in the return it delete punctuation and covert the string to lower case
        try {
            BufferedReader reader = new BufferedReader(new FileReader(filename));
            StringBuilder output = new StringBuilder();
            String line;
            while ((line = reader.readLine()) != null) {
                output.append(line);
                output.append(" "); //used to separate each word from the others
            }
            reader.close();
            return output.toString().toLowerCase();
        } catch (IOException e) {
            System.err.format("Exception occurred trying to read '%s'.", filename);
            e.printStackTrace();
            return null;
        }
    }

    public static void myWriteFile(String fileName,List<String> text) {
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

    public static List<String> generateNgrams(int n, String str) {
        //function that compute n-grams from a string that contains all words
        String letters = "abcdefghijklmnopqrstuvwxyz";
        boolean skip;
        List<String> ngrams = new ArrayList<>();
        for (int i = 0; i < str.length() - n + 1; i++) {
            skip = false;
            for (int k = 0; k < n ; k++) {
                //not consider n-grams with spaces or numbers
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

    public static void frequency(List<String> list_ngrams) {
        //function that compute the frequence of each n-gram in the selected text
        LinkedHashMap<String, Float> freq_dict = new LinkedHashMap<>();
        int tot;
        for (int i = 0 ; i < list_ngrams.size() ; i++) {
            //if n-gram is in the hashmap skip to the next one.
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
        System.out.println(freq_dict);
    }


    public static void main(String[] Args) {
        String book = myReadFile("orgoglio_64mb.txt");

        assert book != null;
        Instant start_t = Instant.now();
        List<String> n_grams = generateNgrams(3, book);

        Instant end_t = Instant.now();
        Duration timeElapsed = Duration.between(start_t, end_t);
        System.out.println("Time taken: "+ timeElapsed.toMillis() +" milliseconds");

        //frequency(n_grams);

    }
}
