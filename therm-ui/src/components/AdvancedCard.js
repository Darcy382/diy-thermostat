import React from "react";
import { Button, Card, Col, Form, Row } from "react-bootstrap";

class AdvancedCard extends React.Component {
  state = {isLoading: false}

  render() {
    return (
    <Card>
      <Card.Body>
        <Card.Title>Advanced:</Card.Title>
        <span>
        <h6>Temperature bound: </h6>
        <Form.Control style={{width:"100px", display: "inline", margin: "20px"}} label="hello" type="number" />
        <Button variant="success">
          Send
        </Button>
        </span>
          <a href="http://192.168.1.25:5000/thermostat/logging" class="btn btn-primary" role="button">
            Download Thermostat logging csv file
          </a>
      </Card.Body>
    </Card>
  );
  }
}

export default AdvancedCard;