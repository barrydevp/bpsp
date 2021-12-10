package com.core.frame;

import com.resources.Operation;

public class FrameFactory {
    private FrameFactory() {
    }

    public static Frame newFrame(Operation operation) {
        return FrameFactory.newFrame(operation, false);
    }

    public static Frame newFrame(Operation operation, boolean build) {
        Frame frame;

        switch (operation) {
            case CONNECT:
                frame = new Frame(Operation.CONNECT.getCode());
                break;
            case PUB:
                frame = new Frame(Operation.PUB.getCode());
                break;
            case SUB:
                frame = new Frame(Operation.SUB.getCode());
                break;
            case UNSUB:
                frame = new Frame(Operation.UNSUB.getCode());
                break;
            default:
                frame = new Frame(operation.getCode());
        }

        if (build) {
            frame.build();
        }

        return frame;
    }

}
