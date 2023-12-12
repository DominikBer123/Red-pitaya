var intChange = function(event) {
    SM.parametersCache["SS_INT_P"] = { value: $("#SS_INT_P").val() };
    SM.sendParameters();
}


var boolChange = function(event) {
    SM.parametersCache["SS_BOOL_P"] = {
        value: $("#SS_BOOL_P option:selected").val() ? true : false
    };
    SM.sendParameters();
}


//Create callback
var changeCallbacks = {}

changeCallbacks["SS_INT_P"] = intChange;
changeCallbacks["SS_BOOL_P"] = boolChange;

var clickCallbacks = {}


//Subscribe changes and clicks
$(document).ready(function() {
    for (var k in changeCallbacks) {
        $("#" + k).change(changeCallbacks[k]);
    }
    for (var i in clickCallbacks) {
        $("#" + i).click(clickCallbacks[i]);
    }
})