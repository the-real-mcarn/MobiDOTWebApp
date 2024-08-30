/**
 * MobiDOT Embedded webserver drawing software
 * Arne van Iterson, 2023
 * https://arnweb.nl
 */

/**
 * Size of display
 */
const W = 21;
const H = 14;

/**
 * Generate dot elements
 */
let t = document.getElementById("pixels");
for (let i = 0; i < W * H; i++) {
    if ((i != 0) & (i % W) == 0) {
        t.appendChild(document.createElement("br"));
    }
    let dot = document.createElement("span");
    dot.addEventListener("mouseenter", (e) => {
        if (e.buttons == 1) {
            set(dot);
        }
    })
    dot.addEventListener("mousedown", () => {
        set(dot);
    })
    t.appendChild(dot);
}

async function post(url, data) {
    var xhr = new XMLHttpRequest();
    xhr.open("POST", '/command' + url, true);
    xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8");

    xhr.onreadystatechange = () => {
        if (xhr.readyState === 4) {
            return;
        }
    }

    let urlencoded = ""
    Object.keys(data).forEach(key => {
        console.log(key, data[key]);
        if (urlencoded != "") {
            urlencoded += "&";
        }
        urlencoded = urlencoded + key + "=" + data[key].toString();
    });
    xhr.send(urlencoded);
}

/**
 * Set the value of a dot
 * @param {} e Dot element
 */
function set(e) {
    e.setAttribute("on", (e.getAttribute("on") == "1") ? "0" : "1")
}

/**
 * Sets all dots to either on or off
 * @param {*} value 0 to all off, 1 to all on
 */
function setAll(value = 0) {
    document.querySelectorAll("div#pixels > span").forEach(dot => {
        dot.setAttribute("on", value ? "1" : "0");
    });
}

function invert() {
    document.querySelectorAll("div#pixels > span").forEach(dot => {
        dot.setAttribute("on", dot.getAttribute("on") == "0" ? "1" : "0");
    });
}

/**
 * Export values of the display
 */
async function send() {
    const bitW = (W % 8) ? W + (8 - (W % 8)) : W; // Round width of display to something devisable by 8
    console.log(bitW);

    const bytes = (bitW * H) / 8; // Amount of bytes in the entire bitmap
    let buffer = Array(bytes).fill(0); // Create a byte buffer of an appropriate size

    // Loop through all the dots
    for (let e = 0; e < (bitW * H); e++) {
        const bytePos = Math.floor(e / 8); // Determine position in the array
        const bitPos = e % 8; // Determine position in the byte

        const linenr = Math.floor(e / bitW); // Current line
        const bitNr = e - (linenr * bitW); // Bit position in line

        // Check if current array position is not padding
        if (bitNr < W) {
            // Determine the value of the dot
            const value = Number(document.querySelectorAll("div#pixels > span")[bitNr + (linenr * W)].getAttribute("on"));

            // Change byte based on value
            if (value) {
                buffer[bytePos] = buffer[bytePos] | (1 << (7 - bitPos));
            }
        }
    }

    console.log(buffer);

    let hexString = "";
    let hexBuffer = buffer.map((byte) => {
        const val = byte.toString(16).padStart(2, '0');
        hexString += val;
        return val;
    });

    let b64buffer = btoa(hexString); 

    console.log(hexBuffer);
    console.log(hexString);
    console.log(b64buffer);

    await post('/base64', { data: b64buffer });

    // let sendBuf = {}
    // let i = 0;

    // for (let index = 0; index < buffer.length; index++) {
    //     if (buffer[index] != 0) {
    //         data = {
    //             [`i${i}`]: index,
    //             [`v${i}`]: buffer[index]
    //         }

    //         sendBuf = {
    //             ...sendBuf,
    //             ...data
    //         };

    //         if (Object.keys(sendBuf).length == 8) {
    //             console.log(sendBuf)
    //             await post('/bitmap', sendBuf);
    //             i = 0;
    //             sendBuf = {};
    //         } else {
    //             i++;
    //         }
    //     }
    // }
    post("/update", 0);
}

/**
 * Add listener for tab switcher
 */
const tabs = document.querySelectorAll("div.tabnav>span");
const content = document.querySelectorAll("div.tab");
tabs.forEach((tab) => {
    tab.addEventListener("click", () => {
        content.forEach((c) => {
            c.classList = "tab hidden"
        })
        tabs.forEach((t) => {
            t.classList = ""
        })
        tab.classList = "active"
        content[Number(tab.getAttribute("tab"))].classList = "tab"
    })
})

/**
 * Add event listeners to buttons
 */
document.getElementById("clear").addEventListener("click", () => setAll(0));
document.getElementById("fill").addEventListener("click", () => setAll(1));
document.getElementById("send").addEventListener("click", () => send());
document.getElementById("invert").addEventListener("click", () => invert());
document.getElementById("toggle").addEventListener("click", () => post('/toggleLight', 0))

/**
 * Set all dots to off on load
 */
window.addEventListener("load", () => setAll(0));