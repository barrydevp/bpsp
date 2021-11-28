import resources.Constants;
import utils.SystemUtils;
import main.client.BpspClient;
import main.frame.Frame;

//**import log4j2 */
import org.apache.logging.log4j.Logger;
import org.apache.logging.log4j.LogManager;

public class App {

    // public App() {
        // super();
    // }

    private static Logger LOGGER = null;
    
    public static void main(String[] args) throws Exception {

        SystemUtils.clearConsole();

        //**set up logger */
        System.setProperty("log4j.configurationFile",  "resources/log4j2.xml"); // set logger config file
        LOGGER = LogManager.getLogger(App.class);

        LOGGER.info("Client began to start");

        BpspClient bpspClient = null; // init bpsp client

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
			bpspClient = new BpspClient(serverIpAddr, serverPort);
            
            Frame connectFrame = new Frame((byte)2,(byte)0,"\"x-topic\"\"locationA\";","hoaidzaivl");
            bpspClient.sendFrame(connectFrame);
            Frame subFrame = new Frame((byte)4,(byte)0,"\"x-topic\"\"locationA\";","");
            bpspClient.sendFrame(subFrame);
            Frame pubFrame = new Frame((byte)3,(byte)0,"\"x-topic\"\"locationA\";","hoai dep trai vl");
            bpspClient.sendFrame(pubFrame);
            Frame unsubFrame = new Frame((byte)5,(byte)0,"\"x-topic\"\"locationA\";","hoai dep trai vl");
            bpspClient.sendFrame(unsubFrame);

            while (true) {
                if (bpspClient.hasData()) {
                    Frame recvFrame = bpspClient.recvFrame();
                    recvFrame.print();
                }
            }
        } catch (Exception e) {
            LOGGER.error(e.getMessage());
            e.printStackTrace();
            bpspClient.stop();
        }
    }
}
