package main;

import java.nio.charset.StandardCharsets;

import lib.Constants;
import lib.Frame;
import lib.FrameFixedHeader;
import lib.SocketClient;

public class Client extends SocketClient
{
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

    public static void main(String args[]) {
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
			Client client = new Client(serverIpAddr, serverPort);
            
            Frame frame = new Frame((byte)2,(byte)0,"\"x-topic\"\"locationA\";","hoaidzaivl");
            client.sendFrame(frame);

            while (true) {
                if (client.in.available() > 0) {
                    Frame recvFrame = client.recvFrame();
                    recvFrame.print();
                }
            }
        } catch (Exception e) {
            System.out.println("Error: " + e.getMessage());
            e.printStackTrace();
        }
    }
}
