package com.core.client;

import com.core.frame.Frame;

import com.core.frame.FrameFactory;
import com.core.frame.VarHeader;
import com.resources.BPSPException;
import com.resources.Constants;
import com.resources.Operation;
import org.apache.logging.log4j.Logger;
import org.apache.logging.log4j.LogManager;

import java.util.HashMap;
import java.util.LinkedList;

public class BPSPClient {

    private static final Logger LOGGER = LogManager.getLogger(BPSPClient.class);

    private String address;
    private int port;
    private BPSPConnection connection;
    private boolean isConnected;
    private volatile boolean _running;
    private Thread loop;
    private LinkedList<Publisher> pendingPub = new LinkedList<>();
    private HashMap<String, Subscriber> subs = new HashMap<>();

    public BPSPClient() {
        this(Constants.DEFAULT_SERVER_IP_ADDR, Constants.DEFAULT_SERVER_PORT);
    }

    // constructor to put ip address and port
    public BPSPClient(String address, int port) {
        this.address = address;
        this.port = port;
        this.isConnected = false;
        this._running = false;
    }

    public void connect() throws Exception {
        if (this.isConnected) {
            LOGGER.warn("Already connect()");
            return;
        }

        try {
            this.connection = new BPSPConnection(address, port);
            this.isConnected = true;

            Frame req = FrameFactory.newFrame(Operation.CONNECT);
            req.putData("BPSP Java Client v1.0.0.");
            req.build();
            this.sendFrame(req);

            Frame res = this.recvFrame();

            if (res.getOpcode() == Operation.INFO.getCode()) {
                LOGGER.info("Connect BPSP Broker Success!");
                LOGGER.info(res.getStrData());
            } else if (res.getOpcode() == Operation.ERR.getCode()) {
                throw new BPSPException("Cannot Connect to Broker: " + res.getStrData());
            }

        } catch (Exception e) {
            LOGGER.error("Error while connecting to BPSP server", e);
            this.isConnected = false;
            if (this.connection != null) {
                this.connection.stop();
                this.connection = null;
            }
            throw e;
        }
    }

    public Frame recvFrame() throws Exception {
        this.checkConnected();

        try {
            Frame frame = Frame.readFrom(this.connection.in);

            return frame;
        } catch (Exception e) {
            LOGGER.error("Error while receiving frame", e);
            throw e;
        }
    }

    public boolean isConnected() {
        return isConnected;
    }

    public void checkConnected() throws BPSPException {
        if (!this.isConnected()) {
            throw new BPSPException("Client is not connected to broker Server, call connect() first.");
        }
    }

    public void sendFrame(Frame frameToSent) throws Exception {
        this.checkConnected();

        try {
            frameToSent.writeTo(this.connection.out);
        } catch (Exception e) {
            LOGGER.error("Error while sending frame", e);
            throw e;
        }
    }

    public void pub(Publisher publisher) throws Exception {
        Frame msg = publisher.buildPubFrame();

        if (msg == null) {
            throw new IllegalArgumentException("Invalid PUB Frame.");
        }

        VarHeader topicHdr = msg.getVarHeader("x-topic");
        String topic = topicHdr != null ? topicHdr.getValue() : null;

        if (topic == null || topic.isEmpty()) {
            throw new IllegalArgumentException("Invalid Publisher Topic.");
        }

        if (msg.getOpcode() != Operation.PUB.getCode()) {
            throw new IllegalArgumentException("Invalid PUB Frame.");
        }

        this.sendFrame(msg);

        this.pendingPub.push(publisher);
    }

    public void sub(Subscriber subscriber) throws Exception {
        String identifier = Subscriber.getIdentifier(subscriber.getTopic(), subscriber.getSubTag());

        Subscriber oldSubscriber = this.subs.get(identifier);
        if (oldSubscriber != null) {
            throw new IllegalArgumentException("Duplicate subscriber(" + identifier + ").");
        }

        Frame sub = subscriber.buildSubFrame();

        if (sub == null || sub.getOpcode() != Operation.SUB.getCode()) {
            throw new IllegalArgumentException("Invalid SUB Frame.");
        }

        VarHeader topicHdr = sub.getVarHeader("x-topic");
        String topic = topicHdr != null ? topicHdr.getValue() : null;

        if (topic == null || topic.isEmpty()) {
            throw new IllegalArgumentException("Invalid Subscriber Topic.");
        }

        this.sendFrame(sub);

        this.subs.put(identifier, subscriber);
    }

    public void unsub(Subscriber subscriber) throws Exception {
        String identifier = Subscriber.getIdentifier(subscriber.getTopic(), subscriber.getSubTag());

        Subscriber oldSubscriber = this.subs.get(identifier);
        if (oldSubscriber != null) {
            return;
        }

        Frame unsub = subscriber.buildUnsubFrame();

        if (unsub == null || unsub.getOpcode() != Operation.UNSUB.getCode()) {
            throw new IllegalArgumentException("Invalid SUB Frame.");
        }

        VarHeader topicHdr = unsub.getVarHeader("x-topic");
        String topic = topicHdr != null ? topicHdr.getValue() : null;

        if (topic == null || topic.isEmpty()) {
            throw new IllegalArgumentException("Invalid Subscriber Topic.");
        }

        this.sendFrame(unsub);

        this.subs.remove(identifier);
    }

    public void startMainLoop() {
        if (this.loop != null) {
            return;
        }

        this.loop = new Thread(new MainLoop());
        this._running = true;

        this.loop.start();
    }

    public Thread getLoop() {
        return this.loop;
    }

    private class MainLoop implements Runnable {

        /**
         * Main looping function, accept frame, send frame, dispatcher to subscriber
         * until _running is false
         */
        @Override
        public void run() {
            boolean shouldDoFinalShutdown = true;

            try {
                while (_running) {
                    Frame recvFrame = BPSPClient.this.recvFrame();
                    recvFrame.print();
                }
            } catch (Throwable ex) {
                if (ex instanceof InterruptedException) {
                    // loop has been interrupted during shutdown,
                    // no need to do it again
//                    shouldDoFinalShutdown = false;
                } else {
//                    handleFailure(ex);
                }
            } finally {
//                if (shouldDoFinalShutdown) {
//                    doFinalShutdown();
//                }
            }
        }
    }
}
