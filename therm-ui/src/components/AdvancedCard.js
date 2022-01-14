import React from "react";
import { Card } from "react-bootstrap";
import { API_HOSTNAME } from "../userSettings";
import TempBoundControl from "./TempBoundControl";
import UseRealFeelSwitch from "./UseRealFeelSwitch";

class AdvancedCard extends React.Component {

  render() {
    return (
    <Card>
      <Card.Body>
        <Card.Title>Advanced:</Card.Title>
        <TempBoundControl tempBound={this.props.tempBound}/>
        <UseRealFeelSwitch useRealFeel={this.props.useRealFeel}/>
        <a href={`http://${API_HOSTNAME}/thermostat/logging`} className="btn btn-primary" role="button">
          Download Thermostat logging csv file
        </a>
      </Card.Body>
    </Card>
  );
  }
}

export default AdvancedCard;