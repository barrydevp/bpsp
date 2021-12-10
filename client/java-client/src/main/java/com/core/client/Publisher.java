package com.core.client;

import com.core.frame.Frame;
import com.core.frame.FrameFactory;
import com.core.frame.VarHeader;
import com.resources.Flag;
import com.resources.Operation;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

public class Publisher extends Handler {
    private static final Logger LOGGER = LogManager.getLogger(Publisher.class);
    private final String topic;
    private String msg;

    public Publisher(BPSPClient client, String topic, String msg) {
        super(client);
        this.topic = topic;
        this.msg = msg;
    }

    public Publisher(BPSPClient client, String topic) {
        this(client, topic, "");
    }

    public boolean isOfFrame(Frame frame) {
        VarHeader topicHdr = frame.getVarHeader("x-topic");
        if (topicHdr == null) {
            return false;
        }

        return topicHdr.getValue().equals(this.topic);
    }

    public void replyOk(Frame reply) {
        this.running = true;
        LOGGER.info("Publish OK on " + this.topic + ".");
    }

    public void replyError(Frame reply) {
        this.running = false;
        LOGGER.error("Publish Error on " + this.topic + ": " + reply.getStrData());
    }

    public void setMsg(String msg) {
        this.msg = msg;
    }

    public Frame buildPubFrame() {
        Frame msg = FrameFactory.newFrame(Operation.PUB);

        msg.setFlag(Flag.ACK);
        msg.setVarHeader("x-topic", this.topic);
        msg.putData(this.msg);
        msg.build();

        return msg;
    }
}
