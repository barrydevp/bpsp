package com.core.client;

import com.core.frame.Frame;
import com.core.frame.FrameFactory;
import com.resources.Operation;

import org.apache.logging.log4j.Logger;
import org.apache.logging.log4j.LogManager;

import java.util.HashMap;

public class PublisherClient {

    private static final Logger LOGGER = LogManager.getLogger(PublisherClient.class);
    private final BPSPClient client;

    public PublisherClient(BPSPClient client) {
        this.client = client;
    }

    public void pub(String topic, String msg) throws Exception {
        this.pub(new Publisher(this.client, topic), msg);
    }

    public void pub(Publisher publisher, String msg) throws Exception {
        try {
            publisher.setMsg(msg);
            this.client.pub(publisher);
        } catch (Exception e) {
            LOGGER.error("Call Publish error", e);
            throw e;
        }
    }
}
