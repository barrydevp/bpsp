package com;

import com.core.frame.Frame;
import com.core.client.SubscriberClient;
import com.core.client.PublisherClient;

import com.resources.Constants;

import org.apache.logging.log4j.Logger;
import org.apache.logging.log4j.LogManager;

import com.utils.SystemUtils;


public class App {

    // public App() {
    // super();
    // }

    private static Logger LOGGER = LogManager.getLogger(App.class);

    public static void main(String[] args) throws Exception {

        SystemUtils.clearConsole();

        LOGGER.info("Client began to start");

        SubscriberClient subClient = null;
        PublisherClient pubClient = null;

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
            subClient = new SubscriberClient(serverIpAddr, serverPort);
            pubClient = new PublisherClient(serverIpAddr, serverPort);

            subClient.connect();
            subClient.sub("locationA");

            pubClient.connect();
            pubClient.pub("locationA", "hoai dep trai");

            while (true) {
                if (subClient.hasData()) {
                    Frame recvFrame = subClient.recvFrame();
                    recvFrame.print();
                }
            }


        } catch (Exception e) {
            LOGGER.error(e.getMessage());
            e.printStackTrace();
            subClient.stop();
            pubClient.stop();
        }
    }
}
