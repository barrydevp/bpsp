package com.core.frame;

import com.resources.Operation;

public class FrameFactory {
	private FrameFactory() {}

	public static final Frame getFrame(Operation operation) {
		switch (operation) {
			case CONNECT:
				return new Frame(Operation.CONNECT.getCode());
			case PUB:
				return new Frame(Operation.PUB.getCode());
			case SUB:
				return new Frame(Operation.SUB.getCode());
			case UNSUB:
				return new Frame(Operation.UNSUB.getCode());
			default:
				return new Frame(operation.getCode());
		}
	}
}
