<script>
import { Line, mixins } from 'vue-chartjs'
import zoom from 'chartjs-plugin-zoom'
const { reactiveProp } = mixins

export default {
  extends: Line,
  mixins: [reactiveProp],
  props: ['chartData', 'timeFrom', 'timeUntil'],
  data () {
    return {
      chartOptions: {
        scales: {
          yAxes: [{
            ticks: {
              beginAtZero: true,
              suggestedMax: 100
            },
            gridLines: {
              display: true
            }
          }],
          xAxes: [ {
            type: 'time',
            time: {
              displayFormats: {
                minute: 'HH:mm',
                hour: 'HH:mm'
              },
              tooltipFormat: 'YYYY-MM-DD HH:mm'
            },
            gridLines: {
              display: false
            }
          }]
        },
        tooltips: {
          backgroundColor: 'rgba(255, 255, 255, 0.8)',
          titleFontColor: '#333',
          displayColors: false,
          bodyFontColor: '#000',
          footerFontColor: '#333'
        },
        legend: {
          display: true
        },
        responsive: true,
        maintainAspectRatio: false,
        pan: {
          enabled: true,
          mode: 'xy'
        }
      }
    }
  },
  mounted () {
    this.addPlugin(zoom)
    this.renderChart(this.chartData, this.chartOptions)
    // this.updateZoom()
  },
  watch: {
    timeFrom () {
      const xOpts = this.$data._chart.options.scales.xAxes[0]
      xOpts.time.min = new Date(this.timeFrom)
      this.$data._chart.update()
    },
    timeUntil () {
      const xOpts = this.$data._chart.options.scales.xAxes[0]
      xOpts.time.max = new Date(this.timeUntil)
      this.$data._chart.update()
    }
  }
}
</script>
