package com.ui;

import com.core.client.BPSPClient;
import com.core.client.Subscriber;
import com.core.frame.Frame;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import javax.swing.*;
import java.awt.*;
import java.util.Collection;
import java.util.HashMap;

public class SmartHomeUI extends AbstractUI {
    private static final Logger LOGGER = LogManager.getLogger(SmartHomeUI.class);

    // UI
    HeaderPanel headerPanel = new HeaderPanel("SMART HOME - BPSP Subscriber UI");
    ConnectPanel connectPanel = new ConnectPanel(this);
    MonitorPanel monitorPanel = new MonitorPanel();


    public SmartHomeUI() {
        this("Smart Home ^^!");
    }

    public SmartHomeUI(String title) {
        super(title);

        this.setLayout(new FlowLayout());
        this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        this.setPreferredSize(new Dimension(AbstractUI.MAX_WIDTH, AbstractUI.MAX_HEIGHT));
        this.setSize(this.getPreferredSize());

        Container c = this.getContentPane();
        c.add(headerPanel);
        c.add(connectPanel);

        c.add(monitorPanel);

        this.setLocationRelativeTo(null);
        this.pack();
        this.setVisible(true);
    }

    @Override
    public void onConnected() {
        LOGGER.info("CONNECT UI.");
//        this.remove(connectPanel);
//        this.repaint();
//        this.revalidate();
        this.monitorPanel.open();
    }

    @Override
    public void onClosed() {
        LOGGER.info("CLOSE UI.");
        this.monitorPanel.close();
    }

    BPSPClient getBPSPClient() {
        return connectPanel.getClient();
    }

    class MonitorPanel extends JPanel {
        static final int BORDER_SIZE = 5;
        static final int MAX_HEIGHT = 380;
        static final int ROOM_CONTAINER_WIDTH = 400;
        static final int ROOM_CONTAINER_HEIGHT = 300;

        // UI
        GridBagConstraints c = new GridBagConstraints();
        DefaultListModel<Room> listModel = new DefaultListModel<>();
        JList<Room> listRoom = new JList<>(listModel);
        NewDeviceFrame newDeviceFrame = new NewDeviceFrame();
        JButton newDeviceBtn = new JButton("New device");
        JPanel roomContainer = new JPanel();

        // Data
        RoomManager roomManager = new RoomManager();

        MonitorPanel() {
            this.setLayout(new GridBagLayout());
            this.setPreferredSize(new Dimension(AbstractUI.MAX_WIDTH, MAX_HEIGHT));
            this.setSize(this.getPreferredSize());
            this.setBorder(BorderFactory.createCompoundBorder(
                    BorderFactory.createEmptyBorder(BORDER_SIZE, BORDER_SIZE, BORDER_SIZE, BORDER_SIZE),
                    BorderFactory.createTitledBorder(BorderFactory.createLineBorder(Color.PINK), "Home Monitoring")
            ));

            listRoom.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
            listRoom.setSelectedIndex(0);
            listRoom.addListSelectionListener(event -> {
                this.roomManager.selectRoom(listRoom.getSelectedValue());
            });

            newDeviceBtn.addActionListener(event -> {
                newDeviceFrame.open();
            });

            c.insets = new Insets(3, 3, 3, 3);

            JPanel controlPanel = new JPanel();
            controlPanel.setPreferredSize(new Dimension(150, 50));
            controlPanel.add(newDeviceBtn);
            this.addToGrid(controlPanel, 0, 0, 0.3, 0.1, 1, 1, GridBagConstraints.BOTH);

            JScrollPane listScroll = new JScrollPane(listRoom);
            listScroll.setPreferredSize(new Dimension(150, 300));
            listScroll.setSize(roomContainer.getPreferredSize());
            this.addToGrid(listScroll, 0, 1, 0.3, 1, 1, 1, GridBagConstraints.BOTH);

            roomContainer.setPreferredSize(new Dimension(ROOM_CONTAINER_WIDTH + 20, ROOM_CONTAINER_HEIGHT + 20));
            roomContainer.setSize(roomContainer.getPreferredSize());
            roomContainer.setBorder(BorderFactory.createLineBorder(Color.ORANGE));
            roomContainer.setVisible(true);

            this.addToGrid(roomContainer, 1, 0, 0.7, 1, 1, 2, GridBagConstraints.BOTH);

            this.setVisible(false);
        }

        void addToGrid(
                Component comp,
                int gridX, int gridY,
                double weightX, double weightY,
                int gridWidth, int gridHeight,
                int fill
        ) {

            c.gridx = gridX;
            c.gridy = gridY;
            c.weightx = weightX;
            c.weighty = weightY;
            c.gridheight = gridHeight;
            c.gridwidth = gridWidth;
            c.fill = fill;
            this.add(comp, c);
        }

        void open() {
            roomManager.clear();
            this.repaint();
            this.revalidate();
            this.setVisible(true);
        }

        void close() {
            this.setVisible(false);
            roomManager.clear();
            listModel.clear();
            listRoom.clearSelection();
        }

        void renderRoomList() {
            listModel.clear();

            for (Room room : roomManager.getRooms()) {
                listModel.addElement(room);
            }

            listRoom.setSelectedIndex(0);
        }

        void addRoomToRenderList(Room room) {
            if (!listModel.contains(room)) {
                listModel.addElement(room);
            }

//            this.listRoom.repaint();
//            this.listRoom.revalidate();
        }

        class NewDeviceFrame extends JFrame {
            static final int BORDER_SIZE = 20;
            static final int MAX_WIDTH = 400;
            static final int MAX_HEIGHT = 400;

            JPanel panel = new JPanel();
            JLabel roomLabel = new JLabel("Room: ");
            JTextField roomField = new JTextField("locationC");
            JLabel deviceLabel = new JLabel("Device Name: ");
            JTextField deviceField = new JTextField("sensorC");
            Button newBtn = new Button("New");

            NewDeviceFrame() {
                super("Add new device");

                panel.setLayout(new GridLayout(0, 2, 5, 1));
                panel.setBorder(BorderFactory.createEmptyBorder(BORDER_SIZE, BORDER_SIZE, BORDER_SIZE, BORDER_SIZE));
//                panel.setPreferredSize(new Dimension(NewDeviceFrame.MAX_WIDTH, NewDeviceFrame.MAX_HEIGHT));
//                panel.setSize(this.getPreferredSize());

                roomField.setColumns(10);
                deviceField.setColumns(10);

                newBtn.addActionListener((event) -> {
                    String roomName = roomField.getText();
                    String deviceName = deviceField.getText();

                    if (roomName.isEmpty() || deviceName.isEmpty()) {
                        return;
                    }

                    RoomManager roomManager = MonitorPanel.this.roomManager;
                    Device device = roomManager.addDevice(roomName, deviceName, "Unknown");

                    if (device == null) {
                        return;
                    }

                    try {
                        device.start();
                        this.setVisible(false);
                    } catch (Exception e) {
                        LOGGER.error(e);
                        JOptionPane.showMessageDialog(
                                this,
                                e.getMessage(),
                                "New Device Error",
                                JOptionPane.ERROR_MESSAGE
                        );
                    }
                });

                panel.add(roomLabel);
                panel.add(roomField);
                panel.add(deviceLabel);
                panel.add(deviceField);
                panel.add(newBtn);

                Container c = this.getContentPane();
                c.add(panel);

                // current frame commit
//            this.setLocationRelativeTo(SmartHomeUI.this);
                this.setLocationRelativeTo(null);
                this.pack();
                this.setVisible(false);
            }

            void open() {
//            this.repaint();
//            this.invalidate();
                this.setVisible(true);
            }

            void close() {
                this.setVisible(false);
            }
        }

        class RoomManager {
            HashMap<String, Room> rooms = new HashMap<>();
            Room currentRoom = null;

            void selectRoom(Room room) {
                if (currentRoom == room) {
                    return;
                }

                if (currentRoom != null) {
                    currentRoom.unmount(MonitorPanel.this.roomContainer);
                }

                currentRoom = room;

                // render to UI
                if (room != null) {
                    room.render(MonitorPanel.this.roomContainer);
                }
            }

            Room getCurrentRoom() {
                return this.currentRoom;
            }

            Collection<Room> getRooms() {
                return this.rooms.values();
            }

            Room getRoom(String roomName) {
                Room room = this.rooms.get(roomName);

                return room;
            }

            Room addRoom(String roomName) {
                Room room = new Room(roomName);
                this.rooms.put(roomName, room);

                // add to UI
                MonitorPanel.this.addRoomToRenderList(room);

                return room;
            }

            Device addDevice(String roomName, String deviceName, String deviceType) {
                Room room = this.getRoom(roomName);

                if (room == null) {
                    room = this.addRoom(roomName);
                }

                return room.addDevice(deviceName, deviceType);
            }

            Device getDevice(String roomName, String deviceName) {
                Room room = this.getRoom(roomName);

                if (room == null) {
                    return null;
                }

                return room.getDevice(deviceName);
            }

            void clear() {
                if (SmartHomeUI.this.getBPSPClient().isConnected()) {
                    for (Room room : rooms.values()) {
                        room.clear();
                    }
                }

                rooms.clear();
            }

        }
    }

    class Room {
        // UI
        JPanel panel = new JPanel();
        JScrollPane container;
        GridBagConstraints c = new GridBagConstraints();

        // Data
        String name;
        HashMap<String, Device> devices = new HashMap<>();

        Room(String name) {
            this.name = name;

            panel.setLayout(new GridBagLayout());
            panel.setBorder(BorderFactory.createEmptyBorder(5, 5, 5, 5));
            container = new JScrollPane(panel);
            container.setBorder(BorderFactory.createEmptyBorder());
            container.setPreferredSize(new Dimension(
                    MonitorPanel.ROOM_CONTAINER_WIDTH + 10,
                    MonitorPanel.ROOM_CONTAINER_HEIGHT + 10)
            );
            container.setSize(panel.getPreferredSize());

            c.gridx = 0;
            c.gridy = 0;
            c.gridwidth = 1;
            c.gridheight = 1;
//            c.fill = GridBagConstraints.HORIZONTAL;

            panel.setVisible(true);
        }

        void render(JPanel container) {
            container.add(this.container);
            container.repaint();
            container.revalidate();
        }

        void unmount(JPanel container) {
            container.remove(this.container);
            container.repaint();
            container.revalidate();
        }

        Device addDevice(String deviceName, String type) {
            return this.addDevice(new Device(this, deviceName, type));
        }

        Device addDevice(String deviceName, String type, String initData) {
            return this.addDevice(new Device(this, deviceName, type, initData));
        }

        Device addDevice(Device device) {
            Device old = this.devices.putIfAbsent(device.getName(), device);

            if (old == null) {
                old = device;
                panel.add(device.getPanel(), c);
                if (c.gridx == 1) {
                    c.gridx = 0;
                    c.gridy++;
                } else {
                    c.gridx++;
                }

                panel.repaint();
                panel.revalidate();
//                if(this.container != null) {
//                    this.container.repaint();
//                    this.container.revalidate();
//                }
            }

            return old;
        }

        public String getName() {
            return name;
        }

        public Device getDevice(String deviceName) {
            return this.devices.get(deviceName);
        }

        void clear() {
            for (Device device : devices.values()) {
                try {
                    device.stop();
                } catch (Exception e) {
                    LOGGER.error(e);
                }
            }

            devices.clear();
        }

        public String toString() {
            return this.name + " : (" + this.devices.size() + ") devices";
        }
    }

    public class Device {
        // UI
        final DevicePanel panel;

        // data
        final Room room;
        final String name;
        final DevicePanel.DeviceType type;
        String data;
        boolean running = false;
        Subscriber subscriber;

        Device(Room room, String name, String type) {
            this(room, name, type, "No data");
        }

        Device(Room room, String name, String type, String initData) {
            this.room = room;
            this.name = name;
            this.data = initData;
            this.type = DevicePanel.DeviceType.getType(type);
            this.panel = DevicePanel.createDevicePanel(this);

            this.subscriber = new DeviceSubscriber(room.getName() + "/" + name);
        }

        public DevicePanel getPanel() {
            return this.panel;
        }

        public String getName() {
            return name;
        }

        public DevicePanel.DeviceType getType() {
            return type;
        }

        public String getData() {
            return data;
        }

        public void start() throws Exception {
            if (!running) {
                SmartHomeUI.this.getBPSPClient().sub(this.subscriber);
                running = true;
            }
        }

        public void stop() throws Exception {
            if (running) {
                SmartHomeUI.this.getBPSPClient().unsub(this.subscriber);
                running = false;
            }
        }

        public void setData(String data) {
            this.data = data;
        }

        class DeviceSubscriber extends Subscriber {
            DeviceSubscriber(String topic) {
                super(SmartHomeUI.this.getBPSPClient(), topic);
            }

            @Override
            public void consumeMsg(Frame msg) {
                super.consumeMsg(msg);

                Device.this.setData(msg.getStrData());
                Device.this.panel.reRender();
            }
        }

    }
}
