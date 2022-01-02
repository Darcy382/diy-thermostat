import { Button, Card, Nav } from "react-bootstrap";
import SchedulePicker from "./SchedulePicker";

function Schedule(props) {
    return (  
    <Card>
      <Card.Body>
        <Card.Title>Schedule:</Card.Title>
        <Nav variant="tabs" defaultActiveKey="heat">
          <Nav.Item>
            <Nav.Link eventKey="heat">Heat</Nav.Link>
          </Nav.Item>
          <Nav.Item>
            <Nav.Link eventKey="cool">Cool</Nav.Link>
          </Nav.Item>
        </Nav>
        <SchedulePicker title="Weekday" schedule={[{start:  7.2, temp: 72}, {start:  7.2, temp: 72}, {start:  7.2, temp: 72}, {start:  7.2, temp: 72}]}/>
        <SchedulePicker title="Weekend" schedule={[{start:  12.6, temp: 72}, {start:  7.2, temp: 72}, {start:  7.2, temp: 72}, {start:  7.2, temp: 72}]}/>
        <Button variant="success">Send to thermostat</Button>
      </Card.Body>
    </Card>
  );
}

export default Schedule;