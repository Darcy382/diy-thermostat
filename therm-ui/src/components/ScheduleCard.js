import React from "react";
import { Button, Card, Nav, Spinner } from "react-bootstrap";
import NotificationManager from "react-notifications/lib/NotificationManager";
import { WEEKDAY_SCHEDULE_HEAT, WEEKEND_SCHEDULE_HEAT, WEEKDAY_SCHEDULE_COOL, WEEKEND_SCHEDULE_COOL } from "./constants";
import ScheduleList from "./ScheduleList";

class ScheduleCard extends React.Component {
  state = {
    activeTab: "heat",
    weekdayScheduleHeat: this.props.weekdayScheduleHeat,
    weekendScheduleHeat: this.props.weekendScheduleHeat,
    weekdayScheduleCool: this.props.weekdayScheduleCool,
    weekendScheduleCool: this.props.weekendScheduleCool,
    isLoading: null
  }

  updateTime = (scheduleType, index, time) => {
    this.setState((prevState) => {
      return {
        [scheduleType]: prevState[scheduleType].map(
          (value, idx) => idx === index ? {...value, start: time} : value
        ),
      }
    })
  }

  updateTemp = (scheduleType, index, temp) => {
    this.setState((prevState) => {
      return {
        [scheduleType]: prevState[scheduleType].map(
          (value, idx) => idx === index ? {...value, temp} : value
        ),
      }
    })
  }


  sendSchedule = async () => {
    this.setState({isLoading: true})
    try {
      const requestOptions = {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ 
          weekdayScheduleHeat: this.state.weekdayScheduleHeat,
          weekendScheduleHeat: this.state.weekendScheduleHeat,
          weekdayScheduleCool: this.state.weekdayScheduleCool,
          weekendScheduleCool: this.state.weekendScheduleCool,
          "time": true 
        })
      };
      const response = await fetch('http://192.168.1.32:5000/thermostat/mode', requestOptions);
      const data = await response.json();
      this.setState({ 
        weekdayScheduleHeat: data.weekdayScheduleHeat, 
        weekendScheduleHeat: data.weekendScheduleHeat,
        weekdayScheduleCool: data.weekdayScheduleCool,
        weekendScheduleCool: data.weekendScheduleCool,
        isLoading: false 
      });
      NotificationManager.success("Success", "", 1000);
    } catch (error) {
      this.setState({ error: error.message, isLoading: false });
      NotificationManager.error("Success", "", 1000);
    }
  }
  
  render() {
    let weekday;
    let weekend;
    let weekdayType;
    let weekendType;

    if (this.state.activeTab === "heat") {
      weekday = this.state.weekdayScheduleHeat
      weekdayType = WEEKDAY_SCHEDULE_HEAT
      weekend = this.state.weekendScheduleHeat
      weekendType = WEEKEND_SCHEDULE_HEAT
    } else {
      weekday= this.state.weekdayScheduleCool
      weekdayType = WEEKDAY_SCHEDULE_COOL
      weekend= this.state.weekendScheduleCool
      weekendType = WEEKEND_SCHEDULE_COOL
    }
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
        <ScheduleList title="Weekday" schedule={weekday} updateTime={this.updateTime} updateTemp={this.updateTemp} scheduleType={weekdayType}/>
        <ScheduleList title="Weekend" schedule={weekend} updateTime={this.updateTime} updateTemp={this.updateTemp} scheduleType={weekendType}/>
        {this.state.isLoading ? (
        <Button variant="outline-secondary" disabled>
          <Spinner animation="border" size="sm" /> <span> Loading...</span>
        </Button>) :
        (<Button onClick={this.sendSchedule} variant="success">
          Send to Thermostat
        </Button>)}
      </Card.Body>
    </Card>
  );
  }
}

export default ScheduleCard;