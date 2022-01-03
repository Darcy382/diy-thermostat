import { mapValues } from "lodash";
import React from "react";
import { Card, Col, Form, Row } from "react-bootstrap";


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
}


class SchedulePicker extends React.Component {

  render() {
    console.log(this.props.schedule)
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
                <Form.Control type="time" step={900} value={hoursToTime(event.start)}/>
                </Col>
                <Col>
                <Form.Control type="number" pattern="\d*" value={event.temp} />
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

export default SchedulePicker;