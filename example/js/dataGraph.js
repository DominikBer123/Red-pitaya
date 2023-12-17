
/**************************************************************************************************/
/*                                        CHARTS                                                  */
/**************************************************************************************************/

//Iteration of graph data
var i = 0;

//Data variables
var distance = 0;
var light = 0;

//PLOT settings
var updateInterval = 1000; //ms
var updateXAxis = updateInterval;
var xAxisRange = 120*1000; //ms = X * 1000 ms = X s


// Test mode -> Chart data will be generated randomly instead of getting data from the hardware
const TEST_MODE = false;

/*------------------------------ NEW DATA (periodical)--------------------------------------------*/
function getNewData()
{
  //Get data from sm.js
  if (TEST_MODE == false) {
    distance = getSMGlobalDistance();
    light = getSMGlobalLight();
  } else {
    distance = Math.floor(Math.random() * 100);
    light = Math.floor(Math.random() * 100);
  }

  //Change view values according to received data
  document.getElementById("distance_text").innerHTML = distance;
  document.getElementById("light_text").innerHTML = light;
}

//Run getNewData() periodically every 1s
window.setInterval(function () {
  getNewData();
}, 1000);


/*--------------------------------- CHART DATA ---------------------------------------------------*/
function getNewChartData(chartNum) {
  if (chartNum == 1) {
    return distance;
  } else if (chartNum == 2) {
    return light;
  } else {
    return 0;
  }
}

/*------------------------------- OTHER FUNCTIONS ------------------------------------------------*/
// Transform date to hh:mm:ss format
function generateMinuteWiseTimeSeries(baseval, count) {
  var i = 0;
  var series = [];
  while (i < count) {
    var x = baseval;
    x += 3600000; // Add 1 hour for UTC+1 (3600000 ms)
    var y = i;
    series.push([x, y]);
    baseval += updateXAxis; //300000;
    i++;
  }
  return series;
}

/*----------------------- CHART SETTINGS ---------------------------------------------------------*/
//Multiple elements if there are multiple lines in the chart
colorPalette = ["#FCCF31", "#17ead9", "#f02fc2"];
strokeWidth = 2;
gradientToColors = ["#f21111", "#6078ea", "#6094ea"];

window.Apex = {
  chart: {
    foreColor: "#fff",
    toolbar: {
      show: false
    }
  },
  colors: colorPalette,
  stroke: {
    width: strokeWidth
  },
  dataLabels: {
    enabled: false
  },
  grid: {
    borderColor: "#40475D"
  },
  xaxis: {
    axisTicks: {
      color: "#333"
    },
    axisBorder: {
      color: "#333"
    }
  },
  fill: {
    type: "gradient",
    gradient: {
      gradientToColors: gradientToColors,
      shadeIntensity: 1,
      inverseColors: true,
      stops: [0, 80, 100],
      type: "vertical"
    }
  },
  tooltip: {
    theme: "dark",
  },
  yaxis: {
    decimalsInFloat: 2,
    opposite: true,
    labels: {
      offsetX: -10
    }
  }
};

var optionsLine1 = {
  chart: {
    height: 350,
    width: 500,
    type: "line",
    stacked: true,
    animations: {
      enabled: false,
      easing: "linear",
    },
    toolbar: {
      show: false
    },
    zoom: {
      enabled: false
    }
  },
  dataLabels: {
    enabled: false
  },
  stroke: {
    curve: "straight",
    width: 3
  },
  grid: {
    padding: {
      left: 0,
      right: 0
    }
  },
  markers: {
    size: 0,
    hover: {
      size: 0
    }
  },
  series: [
    {
      name: "Distance",
      data: generateMinuteWiseTimeSeries(
        new Date().getTime(),
        6
      )    
    }
  ],
  xaxis: {
    type: "datetime",
    range: xAxisRange 
  },
  title: {
    text: "Distance / km",
    align: "left",
    style: {
        fontSize: "15px"
    }
  },
  legend: {
    show: true,
    floating: true,
    horizontalAlign: "left",
    onItemClick: {
      toggleDataSeries: false
    },
    position: "top",
    offsetY: -33,
    offsetX: 60
  }
};

//Copy of the object for the second chart
var optionsLine2 = JSON.parse(JSON.stringify(optionsLine1));
optionsLine2.series.name = "Light";
optionsLine2.title.text = "Light / lux";

/*-------------------------------- CHART ---------------------------------------------------------*/

// Create charts on window load
var chartLine1;
var chartLine2;
window.onload = function() {
  chartLine1 = new ApexCharts(
    document.querySelector("#linechart1"),
    optionsLine1
  );
  chartLine2 = new ApexCharts(
    document.querySelector("#linechart2"),
    optionsLine2
  );
  chartLine1.render();
  chartLine2.render();
}


//Update chart data periodically
window.setInterval(function () {
  //Set all data previous data that apex chart creates to 0
  if(i == 0)
  {
    for(k = 0; k < 6; k++)
    {
      chartLine1.w.config.series[0].data[k][1] = 0;
      chartLine2.w.config.series[0].data[k][1] = 0;
    }
  }

  //Slice out old data 
  if(i < xAxisRange/updateInterval) t = 0;
  else t = 1;
  i++;
  //console.log("i: " + i + "   t: " + t);

  chartLine1.updateSeries([
  {
    data: [
      ...chartLine1.w.config.series[0].data.slice(t),
      [chartLine1.w.globals.maxX + updateXAxis, getNewChartData(1)]
    ]
  }
  ]);

  chartLine2.updateSeries([
  {
    data: [
      ...chartLine2.w.config.series[0].data.slice(t),
      [chartLine2.w.globals.maxX + updateXAxis, getNewChartData(2)]
    ]
  }
  ]);

}, updateInterval);
    
    