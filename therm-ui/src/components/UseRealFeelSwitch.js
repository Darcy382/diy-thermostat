import React from "react";
import { Form, Spinner } from "react-bootstrap";
import NotificationManager from "react-notifications/lib/NotificationManager";
import { API_HOSTNAME } from "../userSettings";

class UseRealFeelSwitch extends React.Component {
  state = {isLoading: false, useRealFeel: this.props.useRealFeel}

  sendUseRealFeel = async (newRealFeelSetting) => {
    let confirmMessage;
    this.setState({isLoading: true});

    if (newRealFeelSetting === true) {
      confirmMessage = "Are you sure you want the thermostat to use real feel temperature?";
      newRealFeelSetting = 1
    } else {
      confirmMessage = "Are you sure you want the thermostat to stop using real feel temperature?";
      newRealFeelSetting = 0
    }

    this.setState({useRealFeel: newRealFeelSetting});
    if (!window.confirm(confirmMessage)) {
      return;
    }

    try {
      const requestOptions = {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body:
          JSON.stringify({
            useRealFeel: newRealFeelSetting
          })
      };
      const response = await fetch(`http://${API_HOSTNAME}/thermostat/mode`, requestOptions);
      const data = await response.json();
      this.setState({ 
        isLoading: false,
        useRealFeel: data.useRealFeel
      });
      const successMessage = data.useRealFeel ? "Thermostat is now using real feel" : "Thermostat is no longer using real feel";
      NotificationManager.success(successMessage, "Success", 1800);
    } catch (error) {
      this.setState({ error: error.message, isLoading: false });
      NotificationManager.error(error.message, "Error", 1800);
    }
  }

  render() {
    return (
      <div className="SettingRow">
      <Form>
        {/* <Form.Label>Use Real Feel: </Form.Label> */}
        <Form.Check 
          style={{marginRight: "0.4rem"}}
          inline
          type="switch"
          id="custom-switch"
          label="Use real feel temperature"
          checked={this.state.useRealFeel}
          onChange={(e) => this.sendUseRealFeel(e.currentTarget.checked)}
        />
        { this.state.isLoading ?
          <Spinner animation="border" size="sm" /> :
          null
        }
      </Form>
      </div>
  );
  }
}

export default UseRealFeelSwitch;