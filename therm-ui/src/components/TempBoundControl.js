import React from "react";
import { Button, Form, InputGroup, Spinner } from "react-bootstrap";
import NotificationManager from "react-notifications/lib/NotificationManager";
import { API_HOSTNAME } from "../userSettings";

class TempBoundControl extends React.Component {
  state = {isLoading: false, tempBound: this.props.tempBound}

  sendTempBound = async () => {
    this.setState({isLoading: true})
    try {
      const requestOptions = {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body:
          JSON.stringify({
            tempBound: parseFloat(this.state.tempBound)
          })
      };
      const response = await fetch(`http://${API_HOSTNAME}/thermostat/mode`, requestOptions);
      const data = await response.json();
      this.setState({ 
        isLoading: false,
        tempBound: data.tempBound
      });
      NotificationManager.success("Temperature bound updated", "Success", 1800);
    } catch (error) {
      this.setState({ error: error.message, isLoading: false });
      NotificationManager.error(error.message, "Error", 1800);

    }
  }

  render() {
    return (
        <div className="SettingRow">
        <Form.Label>Temperature bound:</Form.Label>
        <InputGroup style={{maxWidth:"150px"}}>
          <Form.Control step={0.1} inputMode="decimal" type="number" value={this.state.tempBound} onChange={(e) => this.setState({tempBound: e.currentTarget.value})} />
          {
            this.state.isLoading ?
            (
              <Button variant="outline-secondary" id="basic-addon2">
              <Spinner animation="border" size="sm" />
              </Button>
            ) : (
              <Button onClick={this.sendTempBound} variant="success" id="basic-addon2">
                Send
              </Button>
            )
          }
        </InputGroup>
        </div>
  );
  }
}

export default TempBoundControl;