import React from 'react';
import { ButtonGroup, Card, ToggleButton } from 'react-bootstrap';
import NotificationManager from 'react-notifications/lib/NotificationManager';

const thermModes = [
  { name: 'Heat', value: 1, buttonStyle: 'outline-danger'},
  { name: 'Cool', value: 2, buttonStyle: 'outline-info'},
  { name: 'Fan', value: 3, buttonStyle: 'outline-warning'},
  { name: 'Off', value: 0, buttonStyle: 'outline-dark'},
];

class ThermostatModes extends React.Component {
  state = { radioValue: this.props.radioValue, isLoading: true, error: null };

  // TODO: Figure out why this works and it is not in an arrow function
  async setThermostatMode(mode) {
    try {
      const requestOptions = {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ mode, "time": true })
      };
      const response = await fetch('http://192.168.1.32:5000/thermostat/mode', requestOptions);
      const data = await response.json();
      this.setState({ radioValue: data.mode, isLoading: false });
      NotificationManager.success("Success", "", 1000);
    } catch (error) {
      this.setState({ error: error.message, isLoading: false });
      NotificationManager.error("Error", "", 1000);

    }
  }
  
  render() {
    return (
      <Card>
      <Card.Body>
        <Card.Title>Mode:</Card.Title>
        <div style={{textAlign: "center"}}>
          <ButtonGroup>
            {thermModes.map((radio, idx) => (
              <ToggleButton
                key={idx}
                id={`radio-${idx}`}
                type="radio"
                variant={radio.buttonStyle}
                name="radio"
                value={radio.value}
                checked={this.state.radioValue === radio.value}
                onChange={(e) => {
                  this.setState({radioValue: parseInt(e.currentTarget.value)})
                  this.setThermostatMode(e.currentTarget.value)
                }}
              >
                {radio.name}
              </ToggleButton>
            ))}
          </ButtonGroup>
        </div>
        </Card.Body>
      </Card>
    );
  }
}

export default ThermostatModes;