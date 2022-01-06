import React from "react";
import { Button, Card, Nav, Spinner } from "react-bootstrap";
import NotificationManager from "react-notifications/lib/NotificationManager";

class AdvancedCard extends React.Component {
  state = {isLoading: false}

  downloadFile = async () => {
    this.setState({isLoading: true})
    try {
      const response = await fetch('http://192.168.1.25:5000/thermostat/logging');
      // const data = await response.json();
      this.setState({ 
        isLoading: false 
      });
      // NotificationManager.success("Success", "", 1000);
    } catch (error) {
      this.setState({ error: error.message, isLoading: false });
      NotificationManager.error("Error", "", 1000);
    }
  }
  
  render() {
    return (
    <Card>
      <Card.Body>
        <Card.Title>Advanced:</Card.Title>
        { this.state.isLoading ? 
        (
          <Button onClick={this.downloadFile} disabled>
          <Spinner animation="border" size="sm" /> Loading...
          </Button>
        ) :
        (
          <a href="http://192.168.1.25:5000/thermostat/logging" class="btn btn-primary" role="button">
            Download Thermostat logging csv file
          </a>
          )
        }
      </Card.Body>
    </Card>
  );
  }
}

export default AdvancedCard;