package com;

import com.core.client.BPSPClient;
import com.core.client.Subscriber;
import com.core.client.SubscriberClient;
import com.core.client.PublisherClient;

import com.resources.Constants;

import org.apache.logging.log4j.Logger;
import org.apache.logging.log4j.LogManager;

import com.utils.SystemUtils;


public class App {

    private static Logger LOGGER = LogManager.getLogger(App.class);

    public static void main(String[] args) throws Exception {

        SystemUtils.clearConsole();

        LOGGER.info("Client began to start");

        BPSPClient client = null;
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

            client = new BPSPClient(serverIpAddr, serverPort);
            client.connect();

            // init client and connect to server
            subClient = new SubscriberClient(client);
            pubClient = new PublisherClient(client);

            subClient.sub(new Subscriber(client, "locationA"));

            pubClient.pub("locationA", "hoai dep trai");

            client.startMainLoop();
            client.getLoop().join();

        } catch (Exception e) {
            LOGGER.error(e.getMessage());
            e.printStackTrace();
//            subClient.stop();
//            pubClient.stop();
        }
    }
}
