import React from 'react';
import { ButtonGroup, Card, ToggleButton } from 'react-bootstrap';

class ThermostatModes extends React.Component {
  state = { radioValue: null, isLoading: true, error: null };

  async componentDidMount() {
    try {
      const response = await fetch('http://192.168.1.32:5000/thermostat/mode');
      const data = await response.json();
      this.setState({ radioValue: data.mode.toString(), isLoading: false });

    } catch (error) {
      this.setState({ error: error.message, isLoading: false });
    }
  }

  async setThermostatMode(mode) {
    try {
      const requestOptions = {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ mode })
      };
      const response = await fetch('http://192.168.1.32:5000/thermostat/mode', requestOptions);
      const data = await response.json();
      this.setState({ radioValue: data.mode.toString(), isLoading: false });
    } catch (error) {
      this.setState({ error: error.message, isLoading: false });
    }
  }
  
  render() {
    const thermModes = [
      { name: 'Heat', value: '1', buttonStyle: 'outline-danger'},
      { name: 'Cool', value: '2', buttonStyle: 'outline-info'},
      { name: 'Fan', value: '3', buttonStyle: 'outline-warning'},
      { name: 'Off', value: '0', buttonStyle: 'outline-dark'},
    ];

    return (
      <Card>
      <Card.Body>
        <Card.Title>Sensor Data:</Card.Title>
        <Card.Text style={{textAlign: "center"}}>
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
                  this.setState({radioValue: e.currentTarget.value})
                  this.setThermostatMode(e.currentTarget.value)
                }}
              >
                {radio.name}
              </ToggleButton>
            ))}
          </ButtonGroup>
        </Card.Text>
        </Card.Body>
      </Card>
    );
  }
}

export default ThermostatModes;