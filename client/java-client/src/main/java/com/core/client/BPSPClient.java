package com.core.client;

import com.core.frame.Frame;
import com.core.frame.FrameFixedHeader;

import com.resources.Constants;

import org.apache.logging.log4j.Logger;
import org.apache.logging.log4j.LogManager;

import java.nio.charset.StandardCharsets;

public class BPSPClient extends BPSPConnection {

    private static final Logger LOGGER = LogManager.getLogger(BPSPClient.class);

    // constructor to put ip address and port
    public BPSPClient(String address, int port) {
        super(address, port);
    }

    public Frame recvFrame() throws Exception {
        Frame frame = null;
        try {
//            //**read fixed header */
//            byte[] fixedHeaderBytes = new byte[Constants.FIXED_HEADER_SIZE];
//            fixedHeaderBytes = recvBytes(Constants.FIXED_HEADER_SIZE);
//            FrameFixedHeader fixedHeader = new FrameFixedHeader(fixedHeaderBytes);
//
//            //**get variables header */
//            int fixedHeaderSize = (int) fixedHeader.getVarsHeaderSize();
//            byte[] varsHeaderBytes = recvBytes(fixedHeaderSize);
//            String varHeaders = new String(varsHeaderBytes, StandardCharsets.UTF_8);
//
//            //** get frame data */
//            int dataSize = (int) fixedHeader.getDataSize();
//            byte[] dataBytes = recvBytes(dataSize);
//            String data = new String(dataBytes, StandardCharsets.UTF_8);
//
//            //**build frame */
//            frame = new Frame(fixedHeader, varHeaders, data);
            frame = Frame.readFrom(this.in);

        } catch (Exception e) {
            LOGGER.error("error while receiving frame", e);
            throw e;
        }
        return frame;
    }

    public void sendFrame(Frame frameToSent) throws Exception {
        try {
//            byte[] frame = frameToSent.toByteArray(); // convert frame to byte array
//            sendBytes(frame, 0, frame.length);

            frameToSent.sendTo(this.out);

        } catch (Exception e) {
            LOGGER.error("error while sending frame", e);
            throw e;
        }
    }
}
