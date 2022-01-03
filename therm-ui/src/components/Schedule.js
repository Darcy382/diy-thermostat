import React from "react";
import { Button, Card, Nav } from "react-bootstrap";
import SchedulePicker from "./SchedulePicker";

class Schedule extends React.Component {
  state = {activeTab: "heat"}

  render() {
    const weekday = (this.state.activeTab === "heat") ? this.props.weekdayScheduleHeat : this.props.weekdayScheduleCool
    const weekend = (this.state.activeTab === "heat") ? this.props.weekendScheduleHeat : this.props.weekendScheduleCool

    return (  
    <Card>
      <Card.Body>
        <Card.Title>Schedule:</Card.Title>
        <Nav activeKey={this.state.activeTab} variant="tabs" onSelect={(selectedKey) => this.setState({activeTab: selectedKey})}>
          <Nav.Item>
            <Nav.Link eventKey="heat">Heat</Nav.Link>
          </Nav.Item>
          <Nav.Item>
            <Nav.Link eventKey="cool">Cool</Nav.Link>
          </Nav.Item>
        </Nav>
        <SchedulePicker title="Weekday" schedule={weekday}/>
        <SchedulePicker title="Weekend" schedule={weekend}/>
        <Button variant="success">Send to thermostat</Button>
      </Card.Body>
    </Card>
  );
  }
}

export default Schedule;