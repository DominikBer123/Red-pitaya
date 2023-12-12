(function() {
    var originalAddClassMethod = jQuery.fn.addClass;
    var originalRemoveClassMethod = jQuery.fn.removeClass;
    $.fn.addClass = function(clss) {
        var result = originalAddClassMethod.apply(this, arguments);
        $(this).trigger('activeChanged', 'add');
        return result;
    };
    $.fn.removeClass = function(clss) {
        var result = originalRemoveClassMethod.apply(this, arguments);
        $(this).trigger('activeChanged', 'remove');
        return result;
    }
})();


(function(SM, $, undefined) {

    // Params cache
    SM.params = {
        orig: {},
        local: {}
    };
    var g_PacketsRecv = 0;

    SM.param_callbacks = {};
    SM.parameterStack = [];
    SM.signalStack = [];
    // Parameters cache
    SM.parametersCache = {};

    // App configuration
    SM.config = {};
    SM.config.app_id = 'example';
    SM.config.server_ip = ''; // Leave empty on production, it is used for testing only
    SM.config.start_app_url = (SM.config.server_ip.length ? 'http://' + SM.config.server_ip : '') + '/bazaar?start=' + SM.config.app_id;
    SM.config.stop_app_url = (SM.config.server_ip.length ? 'http://' + SM.config.server_ip : '') + '/bazaar?stop=' + SM.config.app_id;
    SM.config.socket_url = 'ws://' + (SM.config.server_ip.length ? SM.config.server_ip : window.location.hostname) + '/wss'; // WebSocket server URI

    // App state
    SM.state = {
        socket_opened: false,
        processing: false,
        editing: false,
        trig_dragging: false,
        cursor_dragging: false,
        mouseover: false,
        resized: false,
        graph_grid_height: null,
        graph_grid_width: null,
        demo_label_visible: false,
        cursor_dragging: false
    };


    SM.startApp = function() {
        $.get(
                SM.config.start_app_url
            )
            .done(function(dresult) {
                if (dresult.status == 'OK') {
                    try {
                        SM.connectWebSocket();
                        console.log("Load manager");
                    } catch (e) {
                        SM.startApp();
                    }
                } else if (dresult.status == 'ERROR') {
                    console.log(dresult.reason ? dresult.reason : 'Could not start the application (ERR1)');
                    SM.startApp();
                } else {
                    console.log('Could not start the application (ERR2)');
                    SM.startApp();
                }
            })
            .fail(function() {
                console.log('Could not start the application (ERR3)');
                SM.startApp();
            });
    };



    // Creates a WebSocket connection with the web server
    SM.connectWebSocket = function() {

        if (window.WebSocket) {
            SM.ws = new WebSocket(SM.config.socket_url);
            SM.ws.binaryType = "arraybuffer";
        } else if (window.MozWebSocket) {
            SM.ws = new MozWebSocket(SM.config.socket_url);
            SM.ws.binaryType = "arraybuffer";
        } else {
            console.log('Browser does not support WebSocket');
        }

        // Define WebSocket event listeners
        if (SM.ws) {
            SM.ws.onopen = function() {
                console.log('Socket opened');

                SM.state.socket_opened = true;
                SM.sendParameters();
                SM.unexpectedClose = true;

            };

            SM.ws.onclose = function() {
                SM.state.socket_opened = false;
                console.log('Socket closed');
                if (SM.unexpectedClose == true)
                    $('#feedSMck_error').modal('show');
            };

            SM.ws.onerror = function(ev) {
                if (!SM.state.socket_opened)
                    SM.startApp();
                console.log('Websocket error: ', ev);
            };

            SM.ws.onmessage = function(ev) {
                try {
                    var data = new Uint8Array(ev.data);
                    var inflate = pako.inflate(data);
                    var text = String.fromCharCode.apply(null, new Uint8Array(inflate));
                    var receive = JSON.parse(text);

                    //Recieving parameters
                    if (receive.parameters) {
                        SM.parameterStack.push(receive.parameters);
                    }

                    //Recieve signals
                    if (receive.signals) {
                        g_PacketsRecv++;
                        SM.signalStack.push(receive.signals);
                    }

                } catch (e) {
                    console.log(e);
                } finally {}
            };
        }
    };

    // For Firefox
    function fireEvent(obj, evt) {
        var fireOnThis = obj;
        if (document.createEvent) {
            var evObj = document.createEvent('MouseEvents');
            evObj.initEvent(evt, true, false);
            fireOnThis.dispatchEvent(evObj);

        } else if (document.createEventObject) {
            var evObj = document.createEventObject();
            fireOnThis.fireEvent('on' + evt, evObj);
        }
    }


    // Sends to server parameters
    SM.sendParameters = function() {
        if (!SM.state.socket_opened) {
            console.log('ERROR: Cannot save changes, socket not opened');
            return false;
        }

        SM.parametersCache["in_command"] = { value: "send_all_params" };
        SM.ws.send(JSON.stringify({ parameters: SM.parametersCache }));
        SM.parametersCache = {};
        return true;
    };


    //Handlers
    var signalsHandler = function() {
        if (SM.signalStack.length > 0) {
            var sig_1 = SM.signalStack[0]["SS_SIGNAL_1"];
            var sig_2 = SM.signalStack[0]["SS_SIGNAL_2"];

            var sig_sum_1 = 0.0;
            var sig_sum_2 = 0.0;

            for (var i = 0; i < sig_1.size; i++) {
                sig_sum_1 += sig_1.value[i];
            }

            for (var i = 0; i < sig_2.size; i++) {
                sig_sum_2 += sig_2.value[i];
            }
            console.log("SIGNAL SUM1 = " + sig_sum_1 + "; SIGNAL SUM2 = " + sig_sum_2);

            SM.signalStack.splice(0, 1);
        }
        if (SM.signalStack.length > 2)
            SM.signalStack.length = [];
    }

    SM.processParameters = function(new_params) {
        var old_params = $.extend(true, {}, SM.params.orig);

        var send_all_params = Object.keys(new_params).indexOf('send_all_params') != -1;
        for (var param_name in new_params) {
            SM.params.orig[param_name] = new_params[param_name];
            if (SM.param_callbacks[param_name] !== undefined)
                SM.param_callbacks[param_name](new_params);
        }
        // Resize double-headed arrows showing the difference between cursors
    };

    var parametersHandler = function() {
        if (SM.parameterStack.length > 0) {
            SM.processParameters(SM.parameterStack[0]);
            SM.parameterStack.splice(0, 2);
        }
    }

    //Set handlers timers
    setInterval(signalsHandler, 40);
    setInterval(parametersHandler, 1);

    SM.change_text = function(new_params) {
        $("#SS_STRING_P").val(new_params['SS_STRING_P'].value);
    }


    SM.change_int = function(new_params) {
        $("#SS_INT_P").val(new_params['SS_INT_P'].value);
    }

    SM.param_callbacks["SS_STRING_P"] = SM.change_text;
    SM.param_callbacks["SS_INT_P"] = SM.change_int;

}(window.SM = window.SM || {}, jQuery));




// Page onload event handler
$(function() {

    var reloaded = $.cookie("SM_forced_reload");
    if (reloaded == undefined || reloaded == "false") {
        $.cookie("SM_forced_reload", "true");
        window.location.reload(true);
    }


    // Bind to the window resize event to redraw the graph; trigger that event to do the first drawing
    $(window).resize(function() {
        if (SM.ws) {
            SM.sendParameters();
        }
    }).resize();


    // Stop the application when page is unloaded
    $(window).on('beforeunload', function() {
        SM.ws.onclose = function() {}; // disable onclose handler first
        SM.ws.close();
    });

    //button
    $('#PRESS_BUT').on('click', function(ev) {
        SM.parametersCache["SS_INT_P"] = { value: 1 };
        SM.sendParameters();
    });

    // Init help
    Help.init(helpListSM);
    Help.setState("idle");

    SM.startApp();

});