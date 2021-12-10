package com.core.client;

import com.core.frame.Frame;
import com.core.frame.FrameFactory;
import com.core.frame.VarHeader;
import com.resources.Flag;
import com.resources.Operation;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

class SubTagFactory {
    static int count = 0;

    static String genTag() {
        return "_" + ++count;
    }
}

public class Subscriber extends Handler {
    private static final Logger LOGGER = LogManager.getLogger(Subscriber.class);
    private final String topic;
    private final String subTag;

    public Subscriber(BPSPClient client, String topic) {
        this(client, topic, SubTagFactory.genTag());
    }

    public Subscriber(BPSPClient client, String topic, String subTag) {
        super(client);
        this.topic = topic;
        this.subTag = subTag;
    }

    public String getTopic() {
        return topic;
    }

    public String getSubTag() {
        return subTag;
    }

    public static String getIdentifier(String topic, String subTag) {
        return topic + "/" + subTag;
    }

    public Frame buildSubFrame() {
        Frame pub = FrameFactory.newFrame(Operation.SUB);

        pub.setFlag(Flag.ACK);
        // set topic
        pub.setVarHeader("x-topic", this.topic);
        pub.setVarHeader("x-sub-tag", this.subTag);
        pub.build();

        return pub;
    }

    public Frame buildUnsubFrame() {
        Frame unsub = FrameFactory.newFrame(Operation.UNSUB);

        unsub.setFlag(Flag.ACK);
        // set topic
        unsub.setVarHeader("x-topic", this.topic);
        unsub.setVarHeader("x-sub-tag", this.subTag);
        unsub.build();

        return unsub;
    }

    public boolean isOfFrame(Frame frame) {
        VarHeader topicHdr = frame.getVarHeader("x-topic");
        if (topicHdr == null) {
            return false;
        }

        VarHeader subTagHdr = frame.getVarHeader("x-sub-tag");
        if (subTagHdr == null) {
            return false;
        }

        return topicHdr.getValue().equals(this.subTag) && subTagHdr.getValue().equals(this.subTag);
    }

    public void replyOk(Frame reply) {
        this.running = true;
        LOGGER.info("Subscribing on " + this.topic + ",sub_tag(" + this.subTag + ").");
    }

    public void replyError(Frame reply) {
        this.running = false;
        LOGGER.error("Subscribe Error on " + this.topic + ",sub_tag(" + this.subTag + "): " + reply.getStrData());
    }

    public void consumeMsg(Frame msg) {
        VarHeader fromHdr = msg.getVarHeader("x-from");
        VarHeader originTopicHdr = msg.getVarHeader("x-origin-topic");

        String from = fromHdr != null ? fromHdr.getValue() : "Unknown";
        String topic = originTopicHdr != null ? originTopicHdr.getValue() : this.topic;

        LOGGER.info(from + " ->> " + topic + " Received " + msg.getDataSize() + " bytes : " + msg.getStrData());
    }
}
