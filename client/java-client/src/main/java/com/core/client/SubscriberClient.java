package com.core.client;

import com.core.frame.Frame;

import com.core.frame.FrameFactory;
import com.resources.Operation;

import org.apache.logging.log4j.Logger;
import org.apache.logging.log4j.LogManager;

public class SubscriberClient extends BpspClient implements Subscriber {

	private static final Logger LOGGER = LogManager.getLogger(SubscriberClient.class);
	
	public SubscriberClient(String address, int port) {
		super(address, port);
	}

	public void connect() {
		try {
			Frame frame = FrameFactory.getFrame(Operation.CONNECT);

			sendFrame(frame);

			Frame resFrame = recvFrame();

			if (resFrame.getFixedHeader().getOpcode() == Operation.INFO.getCode()) {
				LOGGER.info(Operation.CONNECT.getText() + " verified OK");
			} else if (resFrame.getFixedHeader().getOpcode() == Operation.ERR.getCode()) {
				LOGGER.error("Failed connecting to bpsp server");
			}
		} catch (Exception e) {
			LOGGER.error("error while connecting to bpsp server", e);
		}
	}

	public void sub(String topic) {
		try {
			Frame frame = FrameFactory.getFrame(Operation.SUB);
			frame.setVarHeader("x-topic", topic);

			sendFrame(frame);

			Frame resFrame = recvFrame();

			if (resFrame.getFixedHeader().getOpcode() == Operation.OK.getCode()) {
				LOGGER.info(Operation.SUB.getText() + " verified OK");
			} else if (resFrame.getFixedHeader().getOpcode() == Operation.ERR.getCode()) {
				LOGGER.error("Failed subcribe to bpsp server");
			}
		} catch (Exception e) {
			LOGGER.error("error while connecting to bpsp server", e);
		}
	}

	public void unSub(String topic) {
		try {
			Frame frame = FrameFactory.getFrame(Operation.UNSUB);
			frame.setVarHeader("x-topic", topic);

			sendFrame(frame);

			Frame resFrame = recvFrame();
			
			if (resFrame.getFixedHeader().getOpcode() == Operation.OK.getCode()) {
				LOGGER.info(Operation.UNSUB.getText() + " verified OK");
			} else if (resFrame.getFixedHeader().getOpcode() == Operation.ERR.getCode()) {
				LOGGER.error("Failed unsubcribe to bpsp server");
			}
		} catch (Exception e) {
			LOGGER.error("error while connecting to bpsp server", e);
		}
	}
}
