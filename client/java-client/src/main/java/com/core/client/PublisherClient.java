package com.core.client;

import com.core.frame.Frame;

import com.core.frame.FrameFactory;

import com.resources.Operation;

import org.apache.logging.log4j.Logger;
import org.apache.logging.log4j.LogManager;

public class PublisherClient extends BPSPClient implements Publisher {

	private static final Logger LOGGER = LogManager.getLogger(PublisherClient.class);

	public PublisherClient(String address, int port) {
		super(address, port);
	}

	public void connect() {
		try {
			Frame frame = FrameFactory.getFrame(Operation.CONNECT);

			sendFrame(frame);

			Frame resFrame = recvFrame();

			if (resFrame.getOpcode() == Operation.INFO.getCode()) {
				LOGGER.info(Operation.CONNECT.getText() + " verified OK");
			} else if (resFrame.getOpcode() == Operation.ERR.getCode()) {
				LOGGER.error("Failed connecting to bpsp server");
			}

		} catch (Exception e) {
			LOGGER.error("error while connecting to bpsp server", e);
		}
	}

	public void pub(String topic, String content) {
		try {
			Frame frame = FrameFactory.getFrame(Operation.PUB);

			frame.setVarHeader("x-topic", topic);
			frame.putData(content);

			sendFrame(frame);

			Frame resFrame = recvFrame();

			if (resFrame.getOpcode() == Operation.OK.getCode()) {
				LOGGER.info(Operation.PUB.getText() + " verified OK");
			} else if (resFrame.getOpcode() == Operation.ERR.getCode()) {
				LOGGER.error("Failed publish to bpsp server");
			}
		} catch (Exception e) {
			LOGGER.error("error while connecting to bpsp server", e);
		}
	}
}
