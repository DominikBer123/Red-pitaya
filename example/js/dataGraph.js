
/**************************************************************************************************/
/*                                        CHARTS                                                   */
/**************************************************************************************************/

//Iteration of graph data
var i = 0;

//Current parameter value to be displayed on the chart
var chartVal = 0;

//Data variables
var distance = 0;
var light = 0;

//PLOT settings
var updateInterval = 1000; //ms
var updateXAxis = updateInterval;
var xAxisRange = 60*1000; //ms => 60s

//Time variables
var d = new Date();
var startTime = 0;

/*------------------------------ NEW DATA (periodical)--------------------------------------------*/
function getNewData()
{
  //Get data from sm.js
  distance = getSMGlobalDistance();
  light = getSMGlobalLight();

  chartVal = distance;

  //Change view values according to received data
  document.getElementById("distance_text").innerHTML = distance;
  document.getElementById("light_text").innerHTML = light;
}

//Run getNewData() periodically every 1s
window.setInterval(function () {
  getNewData();
}, 1000);


/*--------------------------------- CHART DATA ---------------------------------------------------*/
function getNewChartData() {
  return chartVal;
}

/*------------------------------- OTHER FUNCTIONS ------------------------------------------------*/
// Transform date to hh:mm:ss format
function generateMinuteWiseTimeSeries(baseval, count) {
  var i = 0;
  var series = [];
  while (i < count) {
    var x = baseval;
    var y = i;
    series.push([x, y]);
    baseval += updateXAxis; //300000;
    i++;
  }
  return series;
}

/*----------------------- CHART SETTINGS ---------------------------------------------------------*/
window.Apex = {
  chart: {
    foreColor: "#fff",
    toolbar: {
      show: false
    }
  },
  colors: ["#FCCF31", "#17ead9", "#f02fc2"],
  stroke: {
    width: 2
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
      gradientToColors: ["#F55555", "#6078ea", "#6094ea"]
    }
  },
  tooltip: {
    theme: "dark",
    x: {
      /* formatter: function (val) {
        return moment(new Date(val)).format("HH:mm:ss");
      } */
    }
  },
  yaxis: {
    decimalsInFloat: 2,
    opposite: true,
    labels: {
      offsetX: -10
    }
  }
};

var optionsLine = {
  chart: {
    height: 350,
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
        //startTime,
        6
      )    
    }
  ],
  xaxis: {
    type: "datetime",
    range: xAxisRange 
  },
  title: {
    text: "Position",
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

/*-------------------------------- CHART ---------------------------------------------------------*/

// Create chart on window load
var chartLine;
window.onload = function() {
  chartLine = new ApexCharts(
    document.querySelector("#linechart"),
    optionsLine
  );
  chartLine.render();
}

//Update chart data periodically
window.setInterval(function () {
  //Set all data previous data that apex chart creates to 0
  if(i == 0)
  {
    for(k = 0; k < 6; k++)
    {
      chartLine.w.config.series[0].data[k][1] = 0;
    }
  }

  //Slice out old data 
  if(i < xAxisRange/updateInterval) t = 0;
  else t = 1;
  i++;
  //console.log("i: " + i + "   t: " + t);

  chartLine.updateSeries([
  {
    data: [
      ...chartLine.w.config.series[0].data.slice(t),
      [chartLine.w.globals.maxX + updateXAxis, getNewChartData()]
    ]
  }
  ]);
}, updateInterval);
    
    