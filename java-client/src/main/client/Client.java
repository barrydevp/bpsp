package main.client;

import resources.Constants;

import main.frame.Frame;
import main.frame.FrameFixedHeader;

import java.nio.charset.StandardCharsets;

public class Client extends SocketClient {
    // constructor to put ip address and port
    public Client(String address, int port)
    {
        super(address, port);
    }
    

    public Frame recvFrame() throws Exception {
        Frame frame = null;
        try {
            //**read fixed header */
            byte[] fixedHeaderBytes = new byte[Constants.FIXED_HEADER_SIZE];
            fixedHeaderBytes = recvBytes(Constants.FIXED_HEADER_SIZE);
            FrameFixedHeader fixedHeader = new FrameFixedHeader(fixedHeaderBytes);

            //**get variables header */
            int fixedHeaderSize = (int)fixedHeader.getVarsHeaderSize();
            byte[] varsHeaderBytes = recvBytes(fixedHeaderSize);
            String varHeaders = new String(varsHeaderBytes, StandardCharsets.UTF_8);

            //** get frame data */
            int dataSize = (int)fixedHeader.getDataSize();
            byte[] dataBytes = recvBytes(dataSize);
            String data = new String(dataBytes, StandardCharsets.UTF_8);

            //**build frame */
            frame = new Frame(fixedHeader, varHeaders, data);

        } catch(Exception e) {
            throw e;
        }
        return frame;
    }

    public void sendFrame(Frame frameToSent) {
        try {
            byte[] frame = frameToSent.toByteArray();
            sendBytes(frame, 0, frame.length);
        } catch(Exception e) {
            throw e;
        }
    }
}
