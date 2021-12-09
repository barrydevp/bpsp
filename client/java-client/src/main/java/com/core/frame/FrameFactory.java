package com.core.frame;

import com.resources.Operation;

public class FrameFactory {
    private FrameFactory() {
    }

    public static Frame getFrame(Operation operation) {
        return FrameFactory.getFrame(operation, true);
    }

    public static Frame getFrame(Operation operation, boolean build) {
        Frame frame = null;

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
