import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;


/**
 * Implements a server which is responsible for sending files
 * @author Ritvaben Vaghasiya
 *
 */
public class Server {
	private Socket socket = null;
	private ServerSocket server = null;
	private int numberOfClients = 0;
	
	public Server(int port) {
		try {
			server = new ServerSocket(port);
			System.out.println("listening for clients at 137.207.82.51:"+port);
			// actively listen for connections
			while(true) {
				socket = server.accept();		
				// create a child process
				numberOfClients++;
				ServiceClient child = new ServiceClient(socket, numberOfClients);
				child.start();
			}
		} catch(IOException io) {
			System.out.println("cannot get input stream: try again later");
		}
		System.out.println("closing connection");
	}
	
	
	public static void main(String[] args) {
		Server server = new Server(4041);
	}
}
