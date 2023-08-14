const SerialPort = require('serialport');
const fs = require("fs");

const port = new SerialPort.SerialPort({
    baudRate: 115200,
    path: "COM3",
    autoOpen: false
}, false);

const waitForData = () => {
    return new Promise((resolve, reject) => {
        const handler = data => {
            clearTimeout(timeout);
            resolve(data.toString("utf-8"));
        };

        let timeout = setTimeout(() => {
            port.removeListener("data", handler);
            reject("Response timeout!");
        }, 5000);

        port.once("data", handler);
    });
};

const data = fs.readFileSync("./fw.bin", { encoding: "hex" }).match(/.{1,2}/g).map(h => "0x" + h);

port.open((async (err) => {
    if (err) throw err;

    console.log("Serial connection port open!");

    port.write("?");
    if ((await waitForData()) !== "READY") {
        console.log("Programmer did not reply with ready signal!");
    }

    console.log("Sending over program bytes in hex..");

    for (const c of data) {
        port.write(c + " ");
        if ((await waitForData().catch(() => null)) !== c) {
            console.log("Programmer did not readback byte correctly!");
            process.exit();
        }
    }

    console.log("All program bytes sent over!\n");

    port.on("data", d => {
        if (d.toString("hex") === "0d0a") return;
        if (d.toString("utf-8").startsWith("FINISH")) {
            console.log("\nProgrammer has notified me that flashing is done - goodbye!");
            process.exit();
        }

        console.log("Message from programmer:", d.toString("utf-8"));
    });

    // Send finish byte.
    port.write(new Uint8Array([13]));
}));
