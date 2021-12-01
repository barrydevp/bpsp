package com;

import com.core.client.BpspClient;
import com.core.frame.Frame;

import com.resources.Constants;

import org.apache.logging.log4j.Logger;
import org.apache.logging.log4j.LogManager;

import com.utils.SystemUtils;


public class Test {

    // public App() {
        // super();
    // }

    private static Logger LOGGER = LogManager.getLogger(Test.class);
    
    public static void main(String[] args) throws Exception {

        SystemUtils.clearConsole();

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
            
            Frame connectFrame = new Frame((byte)2,(byte)0,"","hoaidzaivl");
            bpspClient.sendFrame(connectFrame);
            Frame subFrame = new Frame((byte)4,(byte)0,"\"x-topic\"\"locationA\";","");
            bpspClient.sendFrame(subFrame);
            Frame pubFrame = new Frame((byte)3,(byte)0,"\"x-topic\"\"locationA\";","hoai dep trai vl");
            bpspClient.sendFrame(pubFrame);
            Frame unsubFrame = new Frame((byte)5,(byte)0,"\"x-topic\"\"locationA\";","");
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
