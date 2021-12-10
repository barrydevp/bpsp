package com.core.client;

import com.core.frame.Frame;

import com.core.frame.FrameFactory;
import com.resources.Operation;

import org.apache.logging.log4j.Logger;
import org.apache.logging.log4j.LogManager;

public class SubscriberClient {

    private static final Logger LOGGER = LogManager.getLogger(SubscriberClient.class);
    public BPSPClient client;

    public SubscriberClient(BPSPClient client) {
        this.client = client;
    }

    public Subscriber sub(String topic) throws Exception {
        Subscriber subscriber = new Subscriber(this.client, topic);
        return this.sub(subscriber);
    }

    public Subscriber sub(Subscriber subscriber) throws Exception {
        try {
            this.client.sub(subscriber);

            return subscriber;
        } catch (Exception e) {
            LOGGER.error("Call Subscribe error", e);

            throw e;
        }
    }

    public void unsub(Subscriber subscriber) throws Exception {
        try {
            this.client.unsub(subscriber);
        } catch (Exception e) {
            LOGGER.error("Call Unsubscribe error", e);

            throw e;
        }
    }
}
