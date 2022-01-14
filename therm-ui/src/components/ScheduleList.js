import React from "react";
import { Col, Form, Row } from "react-bootstrap";


function hoursToTime(hours) {
  let hours_int = parseInt(hours)
  let minutes = parseInt((hours - hours_int) * 60)
  let result = ""
  if (hours_int < 10) {
    result = "0"
  }
  result += hours_int.toString()
  result += ":"
  if (minutes < 10) {
    result += "0"
  }
  result += minutes
  return result
}


function timeToHours(time) {
  const hours = parseInt(time.substring(0, 2))
  const minutes = parseInt(time.substring(3))
  return hours + (minutes / 60.);
}

class ScheduleList extends React.Component {

  render() {
    return (
      <div className="schedule" key="">
        <h6>{this.props.title}</h6>
        <Form>
          <Row>
            <Col>
            <p>Time</p>
            </Col>
            <Col>
            <p>Temperature</p>
            </Col>
          </Row>
          {this.props.schedule.map((event, index) => {
            return (
              <div key={index}>
              <Row>
                <Col>
                <Form.Control type="time" step={900} onChange={(event) => this.props.updateTime(this.props.scheduleType, index, timeToHours(event.target.value))} value={hoursToTime(event.start)}/>
                </Col>
                <Col>
                <Form.Control type="number" inputMode="decimal" onChange={(event) => this.props.updateTemp(this.props.scheduleType, index, parseFloat(event.target.value))} value={event.temp} />
                </Col>
              </Row>
              <br />
              </div>
            )
          })}
      </Form>

      </div>
    );
  }
}

export default ScheduleList;