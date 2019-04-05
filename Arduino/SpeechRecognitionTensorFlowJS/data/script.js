let recognizer;

function loadOnOff(url, timeout, state, ONOFF) {
    var args = Array.prototype.slice.call(arguments, 3);
    var xhr = new XMLHttpRequest();
    xhr.ontimeout = function () {
        console.error("The request for " + url + " timed out.");
    };
    xhr.onload = function() {
        if (xhr.readyState === 4) {
            if (xhr.status === 200) {
                state.innerHTML = ONOFF;
            } else {
                console.error(xhr.statusText);
            }
        }
    };
    xhr.open("GET", url, true);
    xhr.timeout = timeout;
    xhr.send(null);
}

function calculateNewPosition(direction) {
    return {
        'up': "on",
        'down': "off",
        'left': "donothing",
        'right': "donothing",
        'default': "donothing"
    }[direction];
}

function predict(state) {
    const words = recognizer.wordLabels();
    recognizer.listen(({ scores }) => {
        scores = Array.from(scores).map((s, i) => ({ score: s, word: words[i] }));
        scores.sort((s1, s2) => s2.score - s1.score);

        var direction = scores[0].word;
        var ONOFF = new String(calculateNewPosition(direction));

        if (ONOFF.valueOf() != new String("donothing").valueOf()) {
            console.log("yay>>" + ONOFF);
            loadOnOff(window.location.protocol + "//" + window.location.host + "/" + ONOFF, 2000, state, ONOFF.toUpperCase());
        } else {
            console.log("nay>>" + ONOFF);
        }
    }, { probabilityThreshold: 0.75 });
}

async function run() {
    recognizer = speechCommands.create('BROWSER_FFT', 'directional4w');
    await recognizer.ensureModelLoaded();
    var state = document.getElementById("state");
    predict(state);
}

run();