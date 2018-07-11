<script>
import {Line, mixins} from 'vue-chartjs'
import hammer from 'hammerjs'
import zoom from 'chartjs-plugin-zoom'
const {reactiveProp} = mixins

export default {
  extends: Line,
  mixins: [reactiveProp],
  props: ['chartData', 'chartOptions', 'zoomLevel'],
  mounted () {
    this.addPlugin(hammer)
    this.addPlugin(zoom)
    this.renderChart(this.chartData, this.chartOptions)
    this.updateZoom()
  },
  watch: {
    zoomLevel () {
      this.updateZoom()
    }
  },
  computed: {
    dateMax () {
      return this.chartData
    },
    timeSpan () {
      let timeSpan // timespan to show on screen [hours]
      switch (this.zoomLevel) {
        case 6:
          timeSpan = 24 * 30 * 3
          break
        case 5:
          timeSpan = 24 * 30
          break
        case 4:
          timeSpan = 24 * 14
          break
        case 3:
          timeSpan = 24 * 7
          break
        case 2:
          timeSpan = 24 * 3
          break
        case 1:
          timeSpan = 24 * 1
          break
      }
      return timeSpan
    }
  },
  methods: {
    updateZoom () {
      const timeSpan = this.timeSpan * 3600 * 1000 // hours -> milliseconds
      const xOpts = this.$data._chart.options.scales.xAxes[0]
      if (xOpts.time.min && xOpts.time.max) {
        const center = (xOpts.time.max.getTime() - xOpts.time.min.getTime()) / 2 + xOpts.time.min.getTime()
        xOpts.time.max = new Date(center + timeSpan / 2)
        xOpts.time.min = new Date(center - timeSpan / 2)
      } else {
        // in case of first init min/max might not be set
        xOpts.time.max = new Date()
        xOpts.time.min = new Date(Date.now() - timeSpan)
      }
      // TODO: check min/max data sets and make sure centering only happens if data is available at both ends of the graph
      this.$data._chart.update()
    },
    // pan left/right by given percentage (-1 .. 1)
    pan (percent = 0) {
      const timeSpan = this.timeSpan * 3600 * 1000
      const xOpts = this.$data._chart.options.scales.xAxes[0]
      xOpts.time.max = new Date(xOpts.time.max.getTime() + percent * timeSpan)
      xOpts.time.min = new Date(xOpts.time.min.getTime() + percent * timeSpan)
      this.$data._chart.update()
    }
  }
}
</script>
