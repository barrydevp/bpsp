import resources.Constants;

import main.client.Client;
import main.frame.Frame;

public class App {
    public static void main(String[] args) throws Exception {
        try {
			// init ip address and port of server
			String serverIpAddr;
			int serverPort;

			// get ip address and port of server
			if (args.length < 2 || args[0].isBlank() || args[1].isBlank()) {
				serverIpAddr = Constants.DEFAULT_SERVER_IP_ADDR;
				serverPort = Constants.DEFAULT_SERVER_PORT;
			} else {
				serverIpAddr = args[0];
				serverPort = Integer.parseInt(args[1]);
			}

			// init client and connect to server
			Client client = new Client(serverIpAddr, serverPort);
            
            Frame frame = new Frame((byte)2,(byte)0,"\"x-topic\"\"locationA\";","hoaidzaivl");
            client.sendFrame(frame);

            while (true) {
                if (client.hasData()) {
                    Frame recvFrame = client.recvFrame();
                    recvFrame.print();
                }
            }
        } catch (Exception e) {
            System.out.println("Error: " + e.getMessage());
            e.printStackTrace();
        }
    }
}
