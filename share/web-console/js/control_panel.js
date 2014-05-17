 
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
        tunnel : { //TODO: add more
            participating: 0
        },
        i2np : { //TODO: add more
            vtb: 0,
            dbsto: 0,
            td: 0,
            tg: 0 
        }
    };
}

var stats_update = function(stats, j_obj)
{
    var bw = j_obj.bandwidth;
    stats.send.value = bw[0] * 8;
    stats.recv.value = bw[1] * 8;
    stats.send.hist.push(stats.send.value);
    stats.recv.hist.push(stats.recv.value);
    stats.hist_len ++;
    stats.hist_count ++;
    while ( stats.hist_len > stats.hist_mean_interval ) {
        stats.send.hist.shift();
        stats.recv.hist.shift();
        stats.hist_len --;
    }
    stats.send.mean = calc_mean(stats.send.hist);
    stats.recv.mean = calc_mean(stats.recv.hist);

    stats.tunnel.participating = j_obj.tunnels.particapting;

    if ( j_obj.i2np.vtb !== undefined ) {
        stats.i2np.vtb = j_obj.i2np.vtb;
    } else {
        stats.i2np.vtb = 0;
    }

    if ( j_obj.i2np.dbsto !== undefined ) {
        stats.i2np.dbsto = j_obj.i2np.dbsto;
    } else {
        stats.i2np.dbsto = 0;
    }

    if ( j_obj.i2np.td !== undefined ) {
        stats.i2np.td = j_obj.i2np.td;
    } else {
        stats.i2np.td = 0;
    }

    if ( j_obj.i2np.tg !== undefined ) {
        stats.i2np.tg = j_obj.i2np.tg;
    } else {
        stats.i2np.tg = 0;
    } 
}

var stats_put_graph = function(stat, graph)
{
    graph.series.addData( { amount: stat.value , mean: stat.mean } );
}


var statsConnection = new WebSocket("ws://127.0.0.1:10010/stats");

statsConnection.onopen = function() { $("#connection_label").html("Connected"); }
statsConnection.onclose = function() { $("#connection_label").html("Disconnected"); }

function make_i2np_graph(elem, h) 
{
    var palette = new Rickshaw.Color.Palette( { scheme: 'munin' } );

    var graph = new Rickshaw.Graph({ 
        element: elem,
        height: h,
        renderer: 'multi',
        
        stroke: true,
        series: new Rickshaw.Series.FixedDuration([
            { name: 'vtb'  , renderer: 'bar' },
            { name: 'dbsto' , renderer: 'bar' },
            { name: 'td'  , renderer: 'bar' },
            { name: 'tg' , renderer: 'bar' },
            ],
            palette,
            {
                timeInterval: 1000,
                maxDataPoints: 180,
                timeBase: new Date().getTime() / 1000
            })
    });

    var hover = new Rickshaw.Graph.HoverDetail( {
        graph: graph,
        formatter: function(series, x, y) {
            return series.name + " : " + y;
        }
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
    var render = function() { x_axis.render(); y_axis.render(); };
    return [graph, render]
}

function make_graph(elem, h) 
{
    var palette = new Rickshaw.Color.Palette( { scheme: 'spectrum14' } );

    var graph = new Rickshaw.Graph({ 
        element: elem,
        height: h,
        renderer: 'multi',
        
        stroke: true,
        series: new Rickshaw.Series.FixedDuration([
            { name: 'mean'  , renderer: 'bar' },
            { name: 'amount' , renderer: 'line' , color: '#33d' }, 
            ],
            palette,
            {
                timeInterval: 1000,
                maxDataPoints: 180,
                timeBase: new Date().getTime() / 1000
            })
    });

    var hover = new Rickshaw.Graph.HoverDetail( {
        graph: graph,
        formatter: function(series, x, y) {
            return make_amount(y);
        }
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
    var render = function() { x_axis.render(); y_axis.render(); };
    return [graph, render]
}

var i2np = make_i2np_graph($("#graph_i2np").get(0), 200);
var render_i2np = i2np[1];
var graph_i2np = i2np[0];

var send = make_graph($("#graph_send").get(0), 200)
var send_axis_render = send[1];
var graph_send = send[0];

send_axis_render();
graph_send.render();

var recv = make_graph($("#graph_recv").get(0), 200);
var recv_axis_render = recv[1];
var graph_recv = recv[0];

recv_axis_render();
graph_recv.render();

var peers = make_graph($("#graph_peers").get(0), 200);
var peers_axis_render = peers[1];
var graph_peers = peers[0];

recv_axis_render();
graph_recv.render();


var update_label = function(stat, elem) 
{
    $(elem).html("Current: " + make_amount(stat.value) + " Mean: " + make_amount(stat.mean));
}

var stats = stats_new();

statsConnection.onmessage = function(msg) 
{
    var data = JSON.parse(msg.data);
    stats_update(stats, data);
    stats_put_graph(stats.recv, graph_recv);
    graph_recv.render();

    stats_put_graph(stats.send, graph_send);
    graph_send.render(); 


    graph_peers.series.addData({ amount: data.peers, mean: 0});
    graph_peers.render();

    update_label(stats.recv, "#recv_str");
    update_label(stats.send, "#send_str");

    $("#peers_str").html(data.peers + " Peers");

    graph_i2np.series.addData(stats.i2np);
    graph_i2np.render();
};
