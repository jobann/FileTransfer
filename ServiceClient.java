import java.net.Socket;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;

public class ServiceClient extends Thread {
    private Socket socket;
    private int id;
    DataInputStream in;
    DataOutputStream out;
    
    public ServiceClient(Socket socket, int id) {
        this.socket = socket;
        this.id = id;
        try {
            this.in = new DataInputStream(socket.getInputStream());
            this.out = new DataOutputStream(socket.getOutputStream());
        } catch(IOException io) {
            System.out.println("could not open input or output stream");
        }
    }
    
    // user enters get command
    public void getFile(String filename, DataOutputStream out) {
        System.out.println("putting file " + filename);
        File file; 
        file = new File(filename);
        if(file.exists() && !file.isDirectory()) {
            System.out.println("file found");
            try {
                out.writeUTF(filename + " found");
                BufferedReader reader = new BufferedReader(new FileReader(file));				
                String line;
                while( (line = reader.readLine()) != null) {
                    out.writeUTF(line);
                }
                out.writeUTF("end");
                out.writeUTF("download complete");
                reader.close();
             } catch(IOException io) {	
                System.out.println("cannnot read the file");
            }
        } else {
            try {					
                out.writeUTF("file not found");
            } catch(IOException io) {
                System.out.println("output error");
            }
            System.out.println("file not found");
        }
    }
    
    // when user puts a file
    public void putFile(DataInputStream in, String filename) {
        try {
            String line;
            File file = new File(filename);
            BufferedWriter fileOutput = new BufferedWriter(new FileWriter(file));
            while(true) { 
                line = in.readUTF();
                if(line.equals("end")) {
                    break;
                }
                fileOutput.write(line + "\n");
            }			
            fileOutput.close();
        } catch(IOException io) {
            System.out.println("input error");
        }
    }
    
    public void run() {
        System.out.println("client:#"+ this.id + " connected...");
        // if input or output stream could not open
        if(this.in == null || this.out == null) {
            return;
        }
        String line;
        while(true) {
            try {
                line = in.readUTF();	
                String splits[] = line.split(" ");

				// Case 1: If user enter the command get
                if(splits[0].equals("get")) {
                    getFile(splits[1], this.out);
                } 
                
                // Case 2: If user enter the command put
                else if(splits[0].equals("put")) {
                    line = in.readUTF();
                    System.out.println(line);
                    putFile(in, splits[1]);
                    line = in.readUTF();
                    System.out.println(line);
                }

                // Case 3: If user enter the command quit                
                else if(splits[0].equals("quit")) {
                    in.close();
                    out.close();
                    socket.close();
                    System.out.println("client:#" + id +" shutdown...");
                    return;
                } 
                // Case 4: if user enter the wrong command
				else{
					System.out.println("Please enter the valid command.");
				}
            } catch(IOException io) {
                System.out.println(io + "could not read from the client");
                return;
            }
        }
    }
}
