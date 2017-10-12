// Game dependent constants
////////////////////////////////////////////////////////////////

// Random number generation
////////////////////////////////////////////////////////////////

var random_seed = 1;

function random() {
    random_seed = (125 * random_seed + 1) % 4096;
    return random_seed;
}


// Viewer state
////////////////////////////////////////////////////////////////

var cur_round = 0; // Current round index

var game_anim    = true;  // Animation is on.
var game_paused  = true;
var game_preview = false; // If true, render will be called for next
  			  // tick even if game is paused, and then
  			  // will be set to false.


// Data
////////////////////////////////////////////////////////////////

var raw_data_str;  // String for storing the raw data to be parsed.
var data = { }     // Object for storing all the game data.


// Animation
////////////////////////////////////////////////////////////////

var SPEED = 25; // Ticks per second
var FRAMES_PER_ROUND = 8;
var frame = 0; // Incremented each tick.
               // When reaches FRAMES_PER_ROUND, cur_round is incremented.


// Visuals
////////////////////////////////////////////////////////////////

// Begin FIXME
var player_colors = {
    '0': "0000FF",  // Blue
    '1': "00FF00",  // Green
    '2': "FF0000",  // Red
    '3': "FFD700",  // Yellow
}
// End FIXME


// Sound fx
////////////////////////////////////////////////////////////////

// Begin FIXME
// var snds = new Object();

// // snds.song    = null;
// snds.song       = new Audio("snd/valkyries.ogg");

// snds.fx_on = (snds.song == null);

// To use them:
    // if (snds.song != null) {
    // 	snds.song.play();
    // }
// End FIXME


// *********************************************************************
// Utility functions
// *********************************************************************


function getURLParameter (name) {
    // http://stackoverflow.com/questions/1403888/get-url-parameter-with-jquery
    var a = (RegExp(name + '=' + '(.+?)(&|$)').exec(location.search)||[,null])[1]
    if (a != null) return decodeURI(a);
    return null;
}


// Callback has a single parameter with the file contents.
function loadFile (file, callback) {

    var xmlhttp;

    if (file == null || file == "") {
        alert("You must specify a file to load");
        return;
    }

    if (window.XMLHttpRequest) {
        // code for IE7+, Firefox, Chrome, Opera, Safari
        xmlhttp = new XMLHttpRequest();
    } else {
        // code for IE6, IE5
        xmlhttp = new ActiveXObject("Microsoft.XMLHTTP");
    }

    // http://www.w3schools.com/ajax/ajax_xmlhttprequest_onreadystatechange.asp
    xmlhttp.onreadystatechange = function() {
        if (xmlhttp.readyState == 4) {
            // Note: We can not check xmlhttp.status != 200 for errors because status is not set when loading local files.
            callback(xmlhttp.responseText);
        }
    }
    xmlhttp.open("GET", file, false);
    xmlhttp.send();
}


function int (s) {
    return parseInt(s);
}


function double (s) {
    return parseFloat(s);
}


function parse_assert(read_value, expected_value) {
    var ok = (read_value == expected_value);
    if (!ok) alert("Error parsing file, expected token: " + expected_value + ", read token: " + read_value);
    return ok;
}


// *********************************************************************
// Initialization functions
// *********************************************************************

function player_of (s) {
    return Math.floor(s / data.nb_ships_x_player);
}


// Begin FIXME
function parseData (raw_data_str) {

    data.tile_size = 50;

    if ("" == raw_data_str) {
        alert("Could not load game file");
        return false;
    }

    // Convert text to tokens.
    var st = raw_data_str + "";
    var t = st.replace('\n', ' ').split(/\s+/);
    var p = 0;

    // Game and version.
    if (t[p++] != "starwar") {
	alert("Are you sure this is a Star War game file?");
	document.getElementById('file').value = "";
	document.getElementById('inputdiv').style.display = "";
	document.getElementById('loadingdiv').style.display = "none";
	return false;
    }

    data.version = t[p++];
    if (data.version != "v1") {
        alert("Unsupported game version! Trying to load it anyway.");
    }

    parse_assert(t[p++], "secgame");
    data.secgame = int(t[p++]);

    parse_assert(t[p++], "seed");
    p++;

    parse_assert(t[p++], "nb_players");
    data.nb_players = int(t[p++]);

    parse_assert(t[p++], "nb_rnds");
    data.nb_rnds = int(t[p++]);

    parse_assert(t[p++], "nb_rows");
    data.nb_rows = int(t[p++]);

    parse_assert(t[p++], "nb_uni_cols");
    data.nb_uni_cols = int(t[p++]);

    parse_assert(t[p++], "nb_win_cols");
    data.nb_win_cols = int(t[p++]);
    data.nb_cols = data.nb_win_cols;

    parse_assert(t[p++], "nb_ships_x_player");
    data.nb_ships_x_player = int(t[p++]);
    data.nb_ships = data.nb_ships_x_player * data.nb_players;

    parse_assert(t[p++], "nb_rnds_regen");    p++;
    parse_assert(t[p++], "nb_miss_bonuses");  p++;
    parse_assert(t[p++], "nb_point_bonuses"); p++;
    parse_assert(t[p++], "bonus_miss");       p++;
    parse_assert(t[p++], "bonus_pnts");       p++;
    parse_assert(t[p++], "kill_pnts");        p++;

    parse_assert(t[p++], "names");
    data.names = new Array();
    for (var i = 0; i < data.nb_players; ++i) {
        data.names[i] = t[p++];
    }

    data.rounds = new Array();
    for (var round = 0; round <= data.nb_rnds; ++round) {

        parse_assert(t[p++], "rnd");
        if (int(t[p++]) != round) alert("Wrong round number!");

        data.rounds[round] = new Object();

        // maze
	p++; // parse_assert(t[p++], "0000000000111111111122222222...");
        p++; // parse_assert(t[p++], "0123456789012345678901234567...");

        data.rounds[round].rows = new Array();
        for (var i = 0; i < data.nb_rows; ++i) {
            var aux = t[p++];
	    var ok = (int(aux) == i);
	    if (!ok) alert("Error parsing file, read token: " + aux);
            data.rounds[round].rows[i] = t[p++];
        }

	// starships
        parse_assert(t[p++], "starships");
        parse_assert(t[p++], "sid");
        parse_assert(t[p++], "row");
        parse_assert(t[p++], "column");
        parse_assert(t[p++], "nb_miss");
        parse_assert(t[p++], "alive");
        parse_assert(t[p++], "time");
	data.rounds[round].ships = new Array();
	for (var s = 0; s < data.nb_ships; ++s) {
	    data.rounds[round].ships[s] = new Object();
	    data.rounds[round].ships[s].sid     = int(t[p++]);
	    data.rounds[round].ships[s].row     = int(t[p++]);
	    data.rounds[round].ships[s].column  = int(t[p++]);
	    data.rounds[round].ships[s].nb_miss = int(t[p++]);
	    data.rounds[round].ships[s].alive   = t[p++];
	    data.rounds[round].ships[s].time    = int(t[p++]);
	}

	// missiles
        parse_assert(t[p++], "missiles");
	data.rounds[round].nb_miss = t[p++];
        parse_assert(t[p++], "mid");
        parse_assert(t[p++], "sid");
        parse_assert(t[p++], "row");
        parse_assert(t[p++], "column");
	data.rounds[round].miss = new Array();
	for (var m = 0; m < data.rounds[round].nb_miss; ++m) {
	    data.rounds[round].miss[m] = new Object();
	    data.rounds[round].miss[m].mid     = int(t[p++]);
	    data.rounds[round].miss[m].sid     = int(t[p++]);
	    data.rounds[round].miss[m].row     = int(t[p++]);
	    data.rounds[round].miss[m].column  = int(t[p++]);
	}

        parse_assert(t[p++], "players");
        parse_assert(t[p++], "pid");
        parse_assert(t[p++], "score");
        parse_assert(t[p++], "status");
        data.rounds[round].players = new Array();
        for (var i = 0; i < data.nb_players; ++i) {
	    data.rounds[round].players[i] = new Object();
            if (int(t[p++]) != i) alert("Wrong player id!");
            data.rounds[round].players[i].score = int(t[p++]);
            var cpu = int(double(t[p++])*100);
            data.rounds[round].players[i].cpu  = (cpu == -100) ? "out" : cpu+"%";
        }

        if (round != data.nb_rnds) {
            // actions asked: skip
            parse_assert(t[p++], "actions_asked");
            for (var pl = 0; pl < data.nb_players; ++pl) {
                if (int(t[p++]) != pl) alert("Wrong player id!");

                var sid = int(t[p++]);
                while (sid != -1) {
		    p+=3;
                    sid = int(t[p++]);
                }
            }

            // actions_done
            parse_assert(t[p++], "actions_done");
            for (var pl = 0; pl < data.nb_players; ++pl) {
                if (int(t[p++]) != pl) alert("Wrong player id!");

                var sid = int(t[p++]);
                while (sid != -1) {
		    p+=3;
                    sid = int(t[p++]);
                }
            }
        }
    }
    return true;
}
// End FIXME


// Initializing the game.
function initGame (raw_data) {

    document.getElementById("loadingdiv").style.display="";

    if (parseData(raw_data) === false) return;
    preloadImages();

    game_paused = false;
    game_preview = true;

    // Slider initialization.
    $("#slider").slider({
	min: 0,
	max: data.nb_rnds,
    });

    // Canvas element.
    canvas = document.getElementById('myCanvas');

    // Prepare the slider.
    $("#slider").slider({
        slide: function(event, ui) {
            var value = $("#slider").slider( "option", "value" );
            cur_round = value;
            frame = 0;
            game_paused = true;
            game_preview = true;
        }
    });
    $("#slider").width(500);

    // set the listerners for interaction
    document.addEventListener('mousewheel', onDocumentMouseWheel, false);
    document.addEventListener('keydown',    onDocumentKeyDown,    false);
    document.addEventListener('keyup',      onDocumentKeyUp,      false);

    window.addEventListener('resize', onWindowResize, false);
    onWindowResize();

    document.getElementById("loadingdiv").style.display="none";
    document.getElementById("gamediv").style.display="";

    main_loop();
}


// Begin FIXME
function preloadImages () {

    data.img = new Array();

    data.img.background = new Image();
    data.img.background.src = "img/space.png";

    data.img.asteroid = new Image();
    data.img.asteroid.src = "img/asteroid.png";

    data.img.miss_bonus = new Image();
    data.img.miss_bonus.src = "img/miss_bonus.png";

    data.img.explosion = new Array();
    for (var k = 0; k < 4; ++k) {
	data.img.explosion[k] = new Image();
	data.img.explosion[k].src = "img/explosion" + k + ".png";
    }

    data.img.starship = new Array();
    for (var p = 0; p < 4; ++p) {
	data.img.starship[p] = new Image();
	data.img.starship[p].src = "img/starship." + player_colors[p] + ".png";
    }

    data.img.miss = new Array();
    for (var p = 0; p < 4; ++p) {
	data.img.miss[p] = new Image();
	data.img.miss[p].src = "img/miss." + player_colors[p] + ".png";
    }

    data.img.point_bonus = new Array();
    data.img.point_bonus[0] = new Image();
    data.img.point_bonus[0].src = "img/point_bonus0.png";
    data.img.point_bonus[1] = new Image();
    data.img.point_bonus[1].src = "img/point_bonus1.png";
    data.img.point_bonus[2] = new Image();
    data.img.point_bonus[2].src = "img/point_bonus2.png";
    data.img.point_bonus[3] = new Image();
    data.img.point_bonus[3].src = "img/point_bonus3.png";
    data.img.point_bonus[4] = new Image();
    data.img.point_bonus[4].src = "img/point_bonus4.png";
    data.img.point_bonus[5] = new Image();
    data.img.point_bonus[5].src = "img/point_bonus3.png";
    data.img.point_bonus[6] = new Image();
    data.img.point_bonus[6].src = "img/point_bonus2.png";
    data.img.point_bonus[7] = new Image();
    data.img.point_bonus[7].src = "img/point_bonus1.png";
}
// End FIXME




// *********************************************************************
// Main loop functions
// *********************************************************************

function writeGameState () {
    // Write round.
    $("#round").html("Round: " + cur_round);

    // Update scoreboard.
    var scoreboard = "";
    for (var i = 0; i <= 2; ++i)
	scoreboard += "<br/><br/>";
    for (var i = 0; i < data.nb_players; i++) {
        scoreboard += "<span class='score'>"
            + "<div style='display:inline-block; margin-top: 5px; width:20px; height:20px; background-color:#"+ player_colors[i] +"'></div>"
            + "<div style='display:inline-block; vertical-align: middle; margin-bottom: 7px; margin-left:8px;'>"+data.names[i]+"</div>"
            + "<br/>"
            + "<div style='margin-left: 10px;'>"
            + "<div style='padding:2px;'>Score: "+data.rounds[cur_round].players[i].score+"</div>"
            + (data.secgame? "<div style='padding:2px;'>CPU: " + (data.rounds[cur_round].players[i].cpu == -1 ? " <font color=\"red\"><b>OUT</b></font>" : data.rounds[cur_round].players[i].cpu) + "</div>" : "")
            + "</div>"
            + "</span><br/><br/><br/>";
    }
    $("#scores").html(scoreboard);
}


function draw(image, i, j) {
    if (!(i >= -1 && i <= data.nb_rows
	    && j >= -1 && j <= data.nb_cols)) return;

    var ctx = canvas.getContext('2d');
    var s = data.tile_size;
    ctx.drawImage(image, j * s, i * s);
}


function circle(i, j, c) {
    if (!(i >= -1 && i <= data.nb_rows
	    && j >= -1 && j <= data.nb_cols)) return;

    var ctx = canvas.getContext('2d');
    var s = data.tile_size;
    var rad = 1.5 * s / 2;
    ctx.beginPath();
    ctx.strokeStyle = "#" + player_colors[c];
    ctx.lineWidth = 5;
    ctx.arc(s*j+s/2, s*i+s/2, rad, 0, 2*Math.PI, true);
    ctx.closePath();
    ctx.stroke();
}


// Begin FIXME
function drawGame () {

    random_seed = 1; // If each round random numbers must behave in the same way.

    // Boundary check.
    if (cur_round < 0) cur_round = 0;
    if (cur_round >= data.nb_rnds) cur_round = data.nb_rnds;

    if (canvas.getContext) {
        var context = canvas.getContext('2d');
        var size = data.tile_size;

	var marginWidth  = 125;
	var marginHeight = 100;

	canvas.width  = window.innerWidth  - marginWidth;
        canvas.height = window.innerHeight - marginHeight;

	var sw = canvas.width  / (size*data.nb_cols);
	var sh = canvas.height / (size*data.nb_rows);
	if (sw < sh) {
	    s = sw;
	    var offset = (canvas.height - s*size*data.nb_rows)/ 2;
	    canvas.style.marginTop   = 0;
	    canvas.style.marginLeft  = marginWidth;
	}
	else {
	    s = sh;
	    var offset = (canvas.width - s*size*data.nb_cols)/ 2;
	    canvas.style.marginTop   = 0;
	    canvas.style.marginLeft  = marginWidth + 2*offset;
	}
        context.scale(s, s);

	var ctx = canvas.getContext('2d');
    	ctx.drawImage(data.img.background, 0, 0, size*data.nb_cols, size*data.nb_rows);

	// Draw asteroids.
	for (var i = 0; i < data.nb_rows; i++) {
	    for (var j = 0; j <= data.nb_cols; ++j) {
		var k = (j + cur_round) % data.nb_uni_cols;
    	        if (data.rounds[cur_round].rows[i][k] == 'X') {
		    if (game_anim && cur_round < data.nb_rnds) {
			if (data.rounds[cur_round+1].rows[i][k] == 'X' || frame < FRAMES_PER_ROUND/2)
			    draw(data.img.asteroid, i, correct(j, j-1));
			else {
			    var idx = frame - FRAMES_PER_ROUND/2;
			    draw(data.img.explosion[idx], i, correct(j, j-1));
			}
		    }
    		    else
			draw(data.img.asteroid, i, j);
		}
	    }
	}

	// Draw point bonuses.
	for (var i = 0; i < data.nb_rows; i++) {
	    for (var j = 0; j <= data.nb_cols; ++j) {
		var k = (j + cur_round) % data.nb_uni_cols;
    	        if (data.rounds[cur_round].rows[i][k] == 'P') {
		    if (game_anim && cur_round < data.nb_rnds) {
			if (data.rounds[cur_round+1].rows[i][k] == 'P' || frame < FRAMES_PER_ROUND/2)
			    draw(data.img.point_bonus[frame], i, correct(j, j-1));
		    }
    		    else
			draw(data.img.point_bonus[4], i, j);
		}
	    }
	}

	// Draw missile bonuses.
	for (var i = 0; i < data.nb_rows; i++) {
	    for (var j = 0; j <= data.nb_cols; ++j) {
		var k = (j + cur_round) % data.nb_uni_cols;
    	        if (data.rounds[cur_round].rows[i][k] == 'M') {
		    if (game_anim && cur_round < data.nb_rnds) {
			if (data.rounds[cur_round+1].rows[i][k] == 'M' || frame < FRAMES_PER_ROUND/2)
			    draw(data.img.miss_bonus, i, correct(j, j-1));
		    }
    		    else
			draw(data.img.miss_bonus, i, j);
		}
	    }
	}
	// Draw starships.
	for (var s = 0; s < data.nb_ships; ++s) {
	    if (data.rounds[cur_round].ships[s].alive == 'y') {
		var r = data.rounds[cur_round].ships[s].row;
		var c = data.rounds[cur_round].ships[s].column % data.nb_uni_cols;
		var rnd = cur_round % data.nb_uni_cols;
		var i = r;
		var j = (c >= rnd) ? c - rnd  :  data.nb_uni_cols - rnd + c;
		if (game_anim && cur_round < data.nb_rnds) {
		    var nr = data.rounds[cur_round+1].ships[s].row;
		    var nc = data.rounds[cur_round+1].ships[s].column % data.nb_uni_cols;
		    var nrnd = (cur_round + 1) % data.nb_uni_cols;
		    var ni = nr;
		    var nj = (nc >= nrnd) ? nc - nrnd  :  data.nb_uni_cols - nrnd + nc;
		    if (data.rounds[cur_round+1].ships[s].alive == 'y' || frame < FRAMES_PER_ROUND/2) {
			draw(data.img.starship[player_of(s)], correct(i, ni), correct(j, nj));
			if (data.rounds[cur_round].ships[s].nb_miss == 0)
			    circle(correct(i, ni), correct(j, nj), player_of(s));
		    }
		    else {
			var idx = frame - FRAMES_PER_ROUND/2;
			draw(data.img.explosion[idx], correct(i, ni), correct(j, nj));
		    }
		}
    		else {
		    draw(data.img.starship[player_of(s)], i, j);
		    if (data.rounds[cur_round].ships[s].nb_miss == 0)
			circle(i, j, player_of(s));
		}
	    }
	}

	// Draw missiles.
	for (var m = 0; m < data.rounds[cur_round].nb_miss; ++m) {
	    var mid = data.rounds[cur_round].miss[m].mid;
	    var sid = data.rounds[cur_round].miss[m].sid;
	    var r =   data.rounds[cur_round].miss[m].row;
	    var c =   data.rounds[cur_round].miss[m].column % data.nb_uni_cols;
	    var rnd = cur_round % data.nb_uni_cols;
	    var i = r;
	    var j = (c >= rnd) ? c - rnd  :  data.nb_uni_cols - rnd + c;
	    if (game_anim && cur_round < data.nb_rnds) {
		var nm = find(data.rounds[cur_round+1].miss, m, mid);
		var nr = r;
		var nc = c+2;
		var nrnd = (cur_round + 1) % data.nb_uni_cols;
		var ni = nr;
		var nj = (nc >= nrnd) ? nc - nrnd  :  data.nb_uni_cols - nrnd + nc;

		if (nm != -1) {
		    if (nj == 0) j = -1;
		    draw(data.img.miss[player_of(sid)], correct(i, ni), correct(j, nj));
		}
		else {
		    var idx = Math.floor(frame / 2);
		    draw(data.img.explosion[idx], i, j);
		}
	    }
    	    else
		draw(data.img.miss[player_of(sid)], i, j);
	}
    }
}
// End FIXME


function find(miss, ini, id) {

    for (var k = Math.min(ini, miss.length-1); k >= 0; --k) {
	if (miss[k].mid == id) return k;
    }
    return -1;
}


function correct (i, f) {
    return i + ((f-i) * frame) / FRAMES_PER_ROUND;
}


// *********************************************************************
// Button events
// *********************************************************************

function playButton () {
    game_paused = false;
}

function pauseButton () {
    game_paused  = true;
    game_preview = true;
    frame = 0;
}

function startButton () {
    cur_round = 0;
    frame = 0;
    game_preview = true;
    game_paused  = true;
}

function endButton () {
    cur_round = data.nb_rnds;
    frame = 0;
    game_preview = true;
}

function animButton () {
    game_anim = !game_anim;
}

function closeButton () {
    window.close();
}


// *********************************************************************
// Keyboard and Mouse events
// *********************************************************************

function onDocumentMouseWheel (event) {
}

function onDocumentKeyDown (event) {
}

function onDocumentKeyUp (event) {

    // http://www.webonweboff.com/tips/js/event_key_codes.aspx

    switch (event.keyCode) {

    case 36: // Start
        cur_round = 0;
        frame = 0;
        game_preview = true;
        break;

    case 35: // End
        cur_round = data.nb_rnds;
        frame = 0;
        game_preview = true;
        break;

    case 33: // PageDown
        cur_round -= 10;
        frame = 0;
        game_preview = true;
        break;

    case 34: // PageUp
        cur_round += 10;
        frame = 0;
        game_preview = true;
        break;

    case 38: // ArrowUp
    case 37: // ArrowLeft
        game_paused= true;
        frame = 0;
        --cur_round;
        game_preview = true;
        break;

    case 40: // ArrowDown
    case 39: // ArrowRight
        game_paused = true;
        frame = 0;
        ++cur_round;
        game_preview = true;
        break;

    case 32: // Space
        if (game_paused) playButton();
        else pauseButton();
        break;

    case 72: // "h"
        help();
        break;

    default:
        break;
    }
}


function onWindowResize (event) {

    // Constants.
    var header_height = 130;
    var canvas_margin = 20;

    // Set canvas size.
    var size = Math.min(document.body.offsetWidth, document.body.offsetHeight - header_height) - canvas_margin*2;

    canvas.width  = size;
    canvas.height = size;

    var max_dimension = Math.max(data.nb_cols, data.nb_rows);
    tile_size = size / max_dimension;

    drawGame();
}


function help () {
    // OPENS a new popup with the help page.
    var win = window.open('help.html' , 'name', 'height=400, width=300');
    if (window.focus) win.focus();
    return false;
}



// *********************************************************************
// This function is called periodically.
// *********************************************************************

function main_loop () {

    // Configure buttons
    if (game_paused) {
        $("#but_play").show();
        $("#but_pause").hide();
    } else {
        $("#but_play").hide();
        $("#but_pause").show();
    }

    if (cur_round < 0) cur_round = 0;
    if (cur_round > data.nb_rnds) {
        cur_round = data.nb_rnds;
        game_paused = true;
        frame = 0;
    }

    if (!game_paused || game_preview) {

        $("#slider").slider("option", "value", cur_round);
        drawGame();
        writeGameState();

        if (game_preview) {
            frame = 0;
            game_preview = false;
        } else {
            frame++;
            if (frame == FRAMES_PER_ROUND) {
                ++cur_round;
                frame = 0;
            }
        }

    }
    // Periodically call main_loop.
    var frame_time = 1000/SPEED;
    setTimeout(main_loop, frame_time);
}





// *********************************************************************
// Main function, it is called when the document is ready.
// *********************************************************************

function init () {

    // get url parameters
    var game;
    if (getURLParameter("sub") != null) {
        var domain = window.location.protocol + "//" + window.location.host;
        if (getURLParameter("nbr") != null) {
            game = domain + "/?cmd=lliuraments&sub="+getURLParameter("sub")+"&nbr="+getURLParameter("nbr")+"&download=partida";
        } else {
            game = domain + "/?cmd=partida&sub="+getURLParameter("sub")+"&download=partida";
        }
    } else {
        game = getURLParameter("game");
    }

    if (game == null || game == "") {
        // ask the user for a game input
        var inputdiv = document.getElementById('inputdiv')
        inputdiv.style.display = "";
        document.getElementById('file').addEventListener('change', function(evt) {
            //http://www.html5rocks.com/en/tutorials/file/dndfiles/
            var file = evt.target.files[0];
            var reader = new FileReader();
            reader.readAsText(file);
            reader.onloadend = function(evt) {
                if (evt.target.readyState == FileReader.DONE) { // DONE == 2
                    inputdiv.style.display = "none";
                    document.getElementById("loadingdiv").style.display="";
                    initGame(reader.result);
                } else {
                    alert("Error accessing file");
                }
            };
        }, false);
    } else {
        document.getElementById("loadingdiv").style.display="";
        // load the given game
        loadFile(game, initGame);
    }

}
