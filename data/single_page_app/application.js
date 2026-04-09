class ConeLightRemote {
  constructor() {
    this.VERSION = "0.1.0";

    this.TX_POWER = 21.0;
    this.PATH_LOSS = 8.25;

    this.CONE_LIGHT_MIN_VOLTAGE = 4.6;
    this.CONE_LIGHT_MAX_VOLTAGE = 5.6;
    this.CONE_LIGHT_VOLTAGE_LOW = 4.8;
    this.CONE_LIGHT_VOLTAGE_MED = 5.1;
    this.CONE_LIGHT_VOLTAGE_HIGH = 5.3;

    this.REMOTE_MIN_VOLTAGE = 4.2;
    this.REMOTE_MAX_VOLTAGE = 3.4;
    this.REMOTE_VOLTAGE_LOW = 3.56;
    this.REMOTE_VOLTAGE_MED = 3.8;
    this.REMOTE_VOLTAGE_HIGH = 3.96;

    // midi piano note labels
    this.MIDI_NOTES = [
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "A0",
      "A#0",
      "B0",
      "C1",
      "C#1",
      "D1",
      "D#1",
      "E1",
      "F1",
      "F#1",
      "G1",
      "G#1",
      "A1",
      "A#1",
      "B1",
      "C2",
      "C#2",
      "D2",
      "D#2",
      "E2",
      "F2",
      "F#2",
      "G2",
      "G#2",
      "A2",
      "A#2",
      "B2",
      "C3",
      "C#3",
      "D3",
      "D#3",
      "E3",
      "F3",
      "F#3",
      "G3",
      "G#3",
      "A3",
      "A#3",
      "B3",
      "C4|MIDDLE C",
      "C#4",
      "D4",
      "D#4",
      "E4",
      "F4",
      "F#4",
      "G4",
      "G#4",
      "A4",
      "A#4",
      "B4",
      "C5",
      "C#5",
      "D5",
      "D#5",
      "E5",
      "F5",
      "F#5",
      "G5",
      "G#5",
      "A5",
      "A#5",
      "B5",
      "C6",
      "C#6",
      "D6",
      "D#6",
      "E6",
      "F6",
      "F#6",
      "G6",
      "G#6",
      "A6",
      "A#6",
      "B6",
      "C7",
      "C#7",
      "D7",
      "D#7",
      "E7",
      "F7",
      "F#7",
      "G7",
      "G#7",
      "A7",
      "A#7",
      "B7",
      "C8",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
    ];

    this.CONE_LIGHT_EMBED_LOGO = document.querySelector(
      "#embed_cone_light_logo",
    ).dataset.blob;
    this.CONE_LIGHT_EMBED_ICON = document.querySelector(
      "#embed_cone_light_icon",
    ).dataset.blob;

    this.DIALOG = document.querySelector("#dialog");

    this.NODES = [];
    this.GROUPS = [];
    this.TONES = [];
    this.SONGS = [];

    this.websocket = null;

    this.initialize();
  }

  initialize() {
    document.querySelectorAll(".cone_light_logo").forEach((item) => {
      item.src = this.CONE_LIGHT_EMBED_LOGO;
    });
    document.querySelectorAll(".cone_light_icon").forEach((item) => {
      item.src = this.CONE_LIGHT_EMBED_ICON;
    });

    Coloris({
      theme: "pill",
      themeMode: "dark",
      alpha: true,
      forceAlpha: true,
      swatches: [
        /** Coloris doesn't preserve current 'alpha' value when selecting a swatch color :'( */
        // "#ff3f00",
        // "#ffbf00",
        // "#bfff00",
        // "#3fff00",
        // "#00ff3f",
        // "#00ffbf",
        // "#00bfff",
        // "#003fff",
        // "#3f00ff",
        // "#bf00ff",
        // "#ff00bf",
        // "#ff003f",
      ],
    });

    const led_auto_update = document.querySelector("#led_control_auto_update");
    document
      .querySelector("#led_control_color")
      .addEventListener("input", (event) => {
        if (led_auto_update.checked) this.handle_led_request();
      });
    document
      .querySelector("#tone_control_select_duration")
      .addEventListener("input", (event) => {
        document.querySelector("#tone_control_duration_label").innerHTML =
          `${event.target.value}ms`;
      });
    document
      .querySelector("#led_control_submit")
      .addEventListener("click", (event) => {
        if (this.websocket && this.websocket.readyState == WebSocket.OPEN) {
          this.handle_led_request();
        }
      });
    document
      .querySelector("#tone_control_submit")
      .addEventListener("click", (event) => {
        if (this.websocket && this.websocket.readyState == WebSocket.OPEN) {
          this.handle_tone_request();
        }
      });
    document
      .querySelector("#song_control_submit")
      .addEventListener("click", (event) => {
        if (this.websocket && this.websocket.readyState == WebSocket.OPEN) {
          this.handle_song_request();
        }
      });

    this.populate_select_node();

    this.connect_websocket();

    console.log(
      "Cone Light Remote v%s initialization completed.",
      this.VERSION,
    );
  }

  connect_websocket() {
    this.DIALOG.showModal();

    this.websocket = new WebSocket(
      `ws://${window.location.host || "192.168.4.1"}/ws`,
    );
    this.websocket.addEventListener("open", (event) => {
      this.DIALOG.close();
      // nuke existing node data
      document.querySelector("#node_groups").innerHTML = "";
      this.NODES = [];

      if (this.interval_id) clearInterval(this.interval_id);

      this.interval_id = setInterval(() => {
        this.update();
      }, 1_000);
    });
    this.websocket.addEventListener("close", (event) => {
      this.DIALOG.showModal();
      // reconnect!
      this.connect_websocket();
    });
    this.websocket.addEventListener("message", (event) => {
      this.DIALOG.close();
      let payload = JSON.parse(event.data);
      // console.log(payload);

      switch (payload.data.type) {
        case "metadata":
          this.handle_metadata(payload);
          break;
        case "payload":
          this.handle_payload(payload);
          break;
      }
    });
  }

  update() {
    if (this.websocket && this.websocket.readyState == WebSocket.OPEN) {
      let data = {};
      data.data = {};
      data.data.command = "payload";

      this.websocket.send(JSON.stringify(data));
    }
  }

  handle_metadata(payload) {
    document.querySelector("#remote_node_version").innerHTML =
      `v${payload.data.metadata.firmware_version} p${payload.data.metadata.protocol_version}`;

    let index = 0;
    for (const group of payload.data.metadata.groups) {
      this.add_node_group(index, group);
      index++;
    }

    this.REMOTE_MIN_VOLTAGE = payload.data.metadata.min_voltage;
    this.REMOTE_MAX_VOLTAGE = payload.data.metadata.max_voltage;

    this.GROUPS = payload.data.metadata.groups;
    this.TONES = payload.data.metadata.tones;
    this.SONGS = payload.data.metadata.songs;

    this.populate_select_tone();
    this.populate_select_song();
  }

  handle_payload(payload) {
    document.querySelector("#remote_node_name").innerHTML =
      `${payload.data.remote.name}:${payload.data.remote.id}:${payload.data.remote.group_id}`;
    document.querySelector("#remote_node_voltage").value =
      payload.data.remote.voltage;
    document.querySelector("#remote_node_voltage").title =
      `${payload.data.remote.voltage.toFixed(3)}v`;
    this.configure_battery_meter(
      document.querySelector("#remote_node_voltage"),
      true,
    );

    if (!payload.data.nodes) payload.data.nodes = [];

    let changed_node = false;
    let node_ids = [];
    for (const node of payload.data.nodes) {
      if (node.id == 255) continue;

      const node_div = document.querySelector(`[data-node_id="${node.id}"]`);
      node_ids.push(node.id);

      if (node_div) {
        this.update_node(node);
      } else {
        changed_node = true;
        this.add_node(node);
      }
    }

    for (const node of this.NODES) {
      if (!node) continue;

      let found = false;

      for (const node_id of node_ids) {
        if (node.id == node_id) {
          found = true;
          break;
        }
      }

      if (!found) this.remove_node(node);
    }

    if (changed_node) this.populate_select_node();
  }

  handle_led_request() {
    const color = document.querySelector("#led_control_color");
    const select_node = document.querySelector("#tone_control_select_node");
    const brightness_only = document.querySelector(
      "#led_control_brightness_only",
    );

    // convert RGBA to ARGB
    let color_value = color.value.replace("#", "").slice(0, 6);
    let brightness_value = color.value.replace("#", "").slice(6, 8);
    let value = parseInt(`${brightness_value}${color_value}`, 16);

    let data = {};
    data.data = {};
    data.data.command = brightness_only.checked ? "brightness" : "color";
    this.set_target_node_or_group(data, "led_control_select_node");
    data.data.parameters = [value];

    this.websocket.send(JSON.stringify(data));

    console.log(data);
  }

  handle_tone_request() {
    const select_tone = document.querySelector("#tone_control_select_tone");
    const duration = document.querySelector("#tone_control_select_duration");
    const select_node = document.querySelector("#tone_control_select_node");

    let data = {};
    data.data = {};
    data.data.command = "tone";
    this.set_target_node_or_group(data, "tone_control_select_node");
    data.data.parameters = [
      parseInt(select_tone.selectedOptions[0].value),
      parseInt(duration.value),
    ];

    this.websocket.send(JSON.stringify(data));

    console.log(data);
  }

  handle_song_request() {
    const select_song = document.querySelector("#song_control_select_song");
    const select_node = document.querySelector("#song_control_select_node");

    let data = {};
    data.data = {};
    data.data.command = "song";
    this.set_target_node_or_group(data, "song_control_select_node");
    data.data.parameters = [parseInt(select_song.selectedOptions[0].value)];

    this.websocket.send(JSON.stringify(data));

    console.log(data);
  }

  set_target_node_or_group(data, select_id) {
    const select = document.querySelector(`#${select_id}`);

    if (Array.from(select.value)[0] === "G") {
      data.data.target_group = parseInt(select.value.replace("G", ""));
    }
    if (Array.from(select.value)[0] === "N") {
      data.data.target_node = parseInt(select.value.replace("N", ""));
    } else {
      data.data.target_node = 255;
    }
  }

  add_node_group(group_id, group_name) {
    const node_groups_div = document.querySelector("#node_groups");

    // .card <div>
    const card_div = document.createElement("div");
    card_div.className = "card";
    card_div.dataset.group_id = group_id;
    // title <b>
    const title_b = document.createElement("b");
    title_b.innerHTML = `${group_id}:${group_name} Group`;
    // .row <div>
    const row_div = document.createElement("div");
    row_div.className = "row";

    card_div.appendChild(title_b);
    card_div.appendChild(row_div);

    node_groups_div.appendChild(card_div);
  }

  add_node(node) {
    this.NODES[node.id] = node;

    const node_group_div = document.querySelector(
      `[data-group_id="${node.group_id}"] .row`,
    );

    // .node <div>
    const node_div = document.createElement("div");
    node_div.className = "node";
    node_div.dataset.node_id = node.id;
    // name <b>
    const name_b = document.createElement("b");
    name_b.innerHTML = `${node.name}:${node.id}:${node.group_id}`;
    // distance <p>
    const distance_p = document.createElement("p");
    distance_p.innerHTML = `${this.meters_to_feet(this.distance_meters(node.rssi)).toFixed(2)}ft`;
    // icon <img>
    const icon_img = document.createElement("img");
    icon_img.src = this.CONE_LIGHT_EMBED_ICON;
    // voltage <meter>
    const voltage_meter = document.createElement("meter");
    this.configure_battery_meter(voltage_meter);
    voltage_meter.value = node.voltage;
    voltage_meter.title = `${node.voltage.toFixed(3)}v`;

    node_div.appendChild(name_b);
    node_div.appendChild(distance_p);
    node_div.appendChild(icon_img);
    node_div.appendChild(voltage_meter);

    node_group_div.appendChild(node_div);
  }

  update_node(node) {
    const distance_p = document.querySelector(`[data-node_id="${node.id}"] p`);
    const voltage_meter = document.querySelector(
      `[data-node_id="${node.id}"] meter`,
    );

    if (distance_p)
      distance_p.innerHTML = `${this.meters_to_feet(this.distance_meters(node.rssi)).toFixed(2)}ft`;

    if (voltage_meter) {
      voltage_meter.value = node.voltage;
      voltage_meter.title = `${node.voltage.toFixed(3)}v`;
    }
  }

  remove_node(node) {
    const node_group_div = document.querySelector(
      `[data-group_id="${node.group_id}"] .row`,
    );
    const node_div = document.querySelector(`[data-node_id="${node.id}"]`);

    if (node_group_div && node_div) {
      delete this.NODES[node.id];
      node_group_div.removeChild(node_div);

      this.populate_select_node();
    }
  }

  populate_select_tone() {
    const select_tone = document.querySelector("#tone_control_select_tone");

    select_tone.innerHTML = "";
    let i = 0;
    for (const tone of this.TONES) {
      const option = document.createElement("option");
      option.value = `${i}`;
      option.text = `${i}. ${this.MIDI_NOTES[i]} ${tone}hz`;

      select_tone.add(option);

      i++;
    }

    if (select_tone.options.length >= 69) {
      select_tone.selectedIndex = 69;
    }
  }

  populate_select_song() {
    const select_song = document.querySelector("#song_control_select_song");

    select_song.innerHTML = "";
    let i = 0;
    for (const song of this.SONGS) {
      const option = document.createElement("option");
      option.value = `${i}`;
      option.text = `${i}. ${song}`;

      select_song.add(option);

      i++;
    }
  }

  populate_select_node() {
    const select_nodes = document.querySelectorAll(".select_node");

    for (const select of select_nodes) {
      select.innerHTML = "";

      // all nodes
      const option_all = document.createElement("option");
      option_all.value = "ALL";
      option_all.text = "ALL";

      select.add(option_all);

      if (this.GROUPS.length > 0) {
        // groups label
        const optgroup_group = document.createElement("optgroup");
        optgroup_group.label = "Node Group";
        select.add(optgroup_group);

        // groups
        let i = 0;
        for (const group of this.GROUPS) {
          const option_group = document.createElement("option");
          option_group.value = `G${i}`;
          option_group.text = `${i}:${group} Group`;

          optgroup_group.appendChild(option_group);

          i++;
        }
      }

      if (this.NODES.length > 0) {
        // nodes label
        const optgroup_name = document.createElement("optgroup");
        optgroup_name.label = "Individual Node";
        select.add(optgroup_name);

        // nodes
        for (const node of this.NODES) {
          if (!node) continue;

          const option_node = document.createElement("option");
          option_node.value = `N${node.id}`;
          option_node.text = `${node.name}:${node.id}:${node.group_id} Node`;

          optgroup_name.appendChild(option_node);
        }
      }
    }
  }

  configure_battery_meter(element, remote) {
    element.min = remote
      ? this.REMOTE_MIN_VOLTAGE
      : this.CONE_LIGHT_MIN_VOLTAGE;
    element.max = remote
      ? this.REMOTE_MAX_VOLTAGE
      : this.CONE_LIGHT_MAX_VOLTAGE;

    element.low = remote
      ? this.REMOTE_VOLTAGE_LOW
      : this.CONE_LIGHT_VOLTAGE_LOW;
    element.high = remote
      ? this.REMOTE_VOLTAGE_MED
      : this.CONE_LIGHT_VOLTAGE_MED;
    element.optimum = remote
      ? this.REMOTE_VOLTAGE_HIGH
      : this.CONE_LIGHT_VOLTAGE_HIGH;
  }

  distance_meters(rssi) {
    let exp = (this.TX_POWER - rssi) / (10 * this.PATH_LOSS);

    return 10 ** exp;
  }

  meters_to_feet(meters) {
    return (meters * 39.37008) / 12.0;
  }
}

window.addEventListener("load", (event) => {
  console.log("Cone Light Remote starting up...");
  const remote = new ConeLightRemote();
});
