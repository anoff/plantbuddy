<script>
import {Line, mixins} from 'vue-chartjs'
import zoom from 'chartjs-plugin-zoom'
const {reactiveProp} = mixins

export default {
  extends: Line,
  mixins: [reactiveProp],
  props: ['chartData', 'chartOptions', 'zoomLevel'],
  mounted () {
    this.addPlugin(zoom)
    this.renderChart(this.chartData, this.chartOptions)
  },
  watch: {
    zoomLevel () {
      console.log('Zoom change', this.zoomLevel)
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
      const xOpts = this.$data._chart.options.scales.xAxes[0]
      // TODO: Orient min/max around the current center
      xOpts.time.max = new Date()
      xOpts.time.min = new Date(Date.now() - timeSpan * 3600 * 1000)
      this.$data._chart.update()
    }
  },
  computed: {
    dateMax () {
      return this.chartData
    }
  }
}
</script>
