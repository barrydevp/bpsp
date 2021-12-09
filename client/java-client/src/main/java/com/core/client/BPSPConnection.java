package com.core.client;

import org.apache.logging.log4j.Logger;
import org.apache.logging.log4j.LogManager;

import java.net.*;

import java.io.*;

public class BPSPConnection {

    private static final Logger LOGGER = LogManager.getLogger(BPSPConnection.class);

    // initialize socket and input output streams
    public Socket socket = null;
    protected DataInputStream in = null;
    protected DataOutputStream out = null;
    private volatile boolean _running = false;

    // constructor to put ip address and port
    public BPSPConnection(String address, int port) {
        // establish a connection
        try {
            socket = new Socket(address, port);

            LOGGER.info("Socket connection established to " + address + " - " + "port");

            // take input stream
            in = new DataInputStream(new BufferedInputStream(socket.getInputStream()));

            // take output stream
            out = new DataOutputStream(new BufferedOutputStream(socket.getOutputStream()));
        } catch (UnknownHostException u) {
            LOGGER.error("unknown host", u);
        } catch (IOException i) {
            LOGGER.error("error while get socket io stream", i);
        }
    }

    public Boolean hasData() {
        try {
            if (in.available() > 0) {
                return true;
            }
        } catch (IOException e) {
            LOGGER.error(e);
        }
        return false;
    }

    protected String recvString() {
        String line = "";
        try {
            line = in.readUTF();
        } catch (IOException i) {
            LOGGER.error(i);
        }
        return line;
    }

    protected void sendString(String message) {
        try {
            out.writeUTF(message);
            out.flush();
        } catch (IOException i) {
            LOGGER.error(i);
        }
    }

    protected byte[] recvBytes(int off, int len) {
        byte[] result = new byte[len];
        try {
            in.readFully(result, off, len);
        } catch (IOException i) {
            LOGGER.error("error while receiving bytes", i);
        }
        return result;
    }

    protected byte[] recvBytes(int len) {
        byte[] result = new byte[len];
        try {
            in.readFully(result, 0, len);
        } catch (IOException i) {
            LOGGER.error("error while receiving bytes", i);
        }
        return result;
    }

    protected void sendBytes(byte[] bytes, int off, int len) {
        try {
            out.write(bytes, off, len);
            out.flush();
        } catch (IOException i) {
            LOGGER.error("error while sending bytes", i);
        }
    }

    public void stop() {
        try {
            // close connection
            in.close();
            out.close();
            socket.close();
            LOGGER.info("Closed connection");
        } catch (IOException i) {
            LOGGER.error("error while stopping socket client");
        }
    }

    private class MainLoop implements Runnable {

        /**
         * Main looping function, accept frame, send frame, dispatcher to suscriber
         * until _running is false
         */
        @Override
        public void run() {
            boolean shouldDoFinalShutdown = true;
            try {
                while (_running) {
//                    Frame frame = _frameHandler.readFrame();
//                    readFrame(frame);
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
