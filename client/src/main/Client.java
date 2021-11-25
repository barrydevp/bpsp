package main;

import java.net.*;

import lib.Constants;
import lib.Frame;

import java.io.*;

public class Client
{
    // initialize socket and input output streams
    public Socket socket            = null;
    private DataInputStream  in   = null;
    private DataOutputStream out     = null;

    public static final int CHUNK_SIZE = 4096;

    // constructor to put ip address and port
    public Client(String address, int port)
    {
        // establish a connection
        try
        {
            socket = new Socket(address, port);
            System.out.println("Connected");

            // take input stream
            in = new DataInputStream(new BufferedInputStream(socket.getInputStream()));
  
            // take output stream
            out = new DataOutputStream(socket.getOutputStream());
        } catch(UnknownHostException u) {
            System.out.println(u);
        } catch(IOException i) {
            System.out.println(i);
        }
    }
    
    public String recvString() {
        String line = "";
        try {
            line = in.readUTF();
            System.out.println(">> Received a message from server: " + line);
        } catch(IOException i) {
            System.out.println(i);
            stop();
        }
        return line;
    }

    public void sendString(String message) {
        try {
            out.writeUTF(message);
            System.out.println(">> Sent a message to server: " + message);
        } catch(IOException i) {
            System.out.println(i);
            stop();
        }
    }

    public byte[] recvBytes(int off, int len) {
        byte[] result = new byte[len];
        try {
            in.readFully(result, off, len);
        } catch(IOException i) {
            System.out.println(i);
            stop();
        }
        return result;
    }

    public void sendBytes(byte[] bytes, int off, int len) {
        try {
            out.write(bytes, off, len);
            System.out.println("Wrote " + len + " bytes to client");
        } catch(IOException i) {
            System.out.println(i);
        }
    }

    public void stop() {
        try {
            // close connection
            in.close();
            out.close();
            socket.close();
            System.out.println("Closed connection");
        } catch(IOException i) {
            System.out.println(i);
        }
    }

    public Frame recvFrame() {
        Frame frame = new Frame();
        try {

        } catch(Exception e) {
            System.out.println(e);
        }
        return frame;
    }

    public void sendFrame(Frame frameToSent) {
        try {
            byte[] frame = frameToSent.toByteArray();
            sendBytes(frame, 0, frame.length);
        } catch(Exception e) {
            System.out.println(e);
        }
    }

    public static void main(String args[]) {

			// init ip address and port of server
			String serverIpAddr;
			int port;

			// get ip address and port of server
			if (args.length < 2 || args[0].isBlank() || args[1].isBlank()) {
				serverIpAddr = Constants.DEFAULT_SERVER_IP_ADDR;
				port = Constants.DEFAULT_SERVER_PORT;
			} else {
				serverIpAddr = args[0];
				port = Integer.parseInt(args[1]);
			}

			// init client and connect to server
			Client client = new Client(serverIpAddr, port);
            
            try {
                Frame frame = new Frame((short)6,(byte)6,(byte)0,10,"\"a\"\"b\"","hoaidzaivl");
                client.sendFrame(frame);
            } catch (Exception e) {}
    }
}
