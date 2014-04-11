 
function make_amount(val) 
{
    if ( val > 1024 ) {
        val = val / 1024;
        if ( val > 1024) {
            return Math.floor(val) + "Mbps";
        }
        return Math.floor(val) + "Kbps";
    }
    return Math.floor(val) + "bps";
}

function calc_mean(arr) 
{
    var s = 0;
    for ( var ind = 0; ind < arr.length; ind++ ) {
        s += arr[ind];                             
    }
    return ( 0.0 + s ) / arr.length;
}

var stats_new = function() 
{
    return {
        send : {
            hist: [],
            mean: 0,
            value: 0,
        },
        recv : {
            hist: [],
            mean: 0,
            value: 0,
        },
        hist_count: 0,
        hist_len: 0,
        hist_mean_interval: 5,
    };
}

var stats_update = function(stats, j_obj)
{
    var bw = j_obj['bandwidth'];
    var peers = j_obj['peers'];
    stats.send.value = bw[0] * 8;
    stats.recv.value = bw[1] * 8;
    stats.send.hist.push(sa);
    stats.recv.hist.push(ra);
    stats.hist_len ++;
    stats.hist_count ++;
    while ( stats.hist_len > stats.hist_mean_interval ) {
        stats.send.hist.shift();
        stats.recv.hist.shift();
        stats.hist_len --;
    }
    stats.recv_mean = calc_mean(stats.send.hist);
    stats.recv_mean = calc_mean(stats.recv.hlist);

}

var stats_put_graph = function(stat, graph)
{
    graph.series.addData( { amount: stat.amount , mean: stat.mean } );
}


var statsConnection = new WebSocket("ws://127.0.0.1:10010/stats");

statsConnection.onopen = function() { }

var g_h = 300;
var g_w = 900;

var palette = new Rickshaw.Color.Palette( { scheme: 'spectrum14' } );

function make_graph(elem_tag) 
{
    var graph = new Rickshaw.Graph({ 
        element: document.querySelector(elem_tag),
        width: g_w,
        height: g_h / 2,
        renderer: 'multi',
        
        stroke: true,
        series: new Rickshaw.Series.FixedDuration([
            { name: 'mean'  , renderer: 'bar' },
            { name: 'amount' , renderer: 'line' , color: '#33d' }, 
            ],
            palette,
            {
                timeInterval: 10000,
                maxDataPoints: 180,
                timeBase: new Date().getTime() / 1000
            })
    });

    var x_axis = new Rickshaw.Graph.Axis.Time({
        graph: graph,
        ticksTreatment: 'glow',
        timeFixture: new Rickshaw.Fixtures.Time.Local()
    })
    var y_axis = new Rickshaw.Graph.Axis.Y({
        graph: graph,
        tickFormat: Rickshaw.Fixtures.Number.formatKMBT,
        ticksTreatment: 'glow'
    });
    return [graph, function() { x_axis.render(); y_axis.render(); }]
}

var graph_send, send_axis_render = make_graph("#graph_send");
var graph_recv, recv_axis_render = make_graph("#graph_recv");

send_axis_render();
graph_send.render();

recv_axis_render();
graph_recv.render();


var update_label = function(stat, elem) 
{
    elem.innerHTML = "Current: " + make_amount(stat.value) + " Mean: " + make_amount(stat.mean);
}

statsConnection.onmessage = function(msg) 
{
    var data = JSON.parse(msg.data);
    stats_update(stats, data);

    stats_put_graph(stats.recv, graph_recv);
    graph_recv.render();

    stats_put_graph(stats.send, graph_send);
    graph_send.render(); 
    
    update_label(stats.recv, $("#recv_str"));
    update_label(stats.send, $("#send_str"));
};
