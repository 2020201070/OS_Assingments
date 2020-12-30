## Assignment-2

  - Tracker: ./track tracker_info (Only one tracker)
  - PEER: ./peer peer_ip peer_port tracker_info
  

##### Commands 

  - create_user <user_id> <passwd>
  - login <user_id> <passwd>
  - create_group <group_id>
  - join_group <group_id>
  - leave_group <group_id>
  - requests list_requests <group_id>
  - accept_request <group_id> <user_id>
  - list_groups
  - list_files <group_id>
  - upload_file <file_path> <group_id >
  - download_file <group_id> <file_name> <destination_path>
  - logout
  - show_downloads


##### Assumption
  - `download`:passing full file path during filename and printing it on console.
  - `show_downloads`:shows complete file[C] and can be downloded file[D]
  - `create_user`: User name should be unique.
  - `leave_group`: Ower cannot leave the group.
  - `create_group `:Only the creater is by default the owner.

