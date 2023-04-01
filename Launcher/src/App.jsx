import { useState, useEffect } from "react";
import { BaseDirectory, readDir, createDir, readTextFile, writeTextFile } from "@tauri-apps/api/fs";
import { invoke } from "@tauri-apps/api/tauri";
import { Command } from '@tauri-apps/api/shell';
import "./App.css";

function App() {
  //look for config.json in the app's directory
    let beencalled = false;
    const [config, setConfig] = useState(null);
    const [projectsDir, setProjectsDir] = useState(null);

    const [projects, setProjects] = useState([]);
    const [projectsLoaded, setProjectsLoaded] = useState(false);
    const [newProjectModalOpen, setNewProjectModalOpen] = useState(false);
  
    const getConfig = async () => {
        await readTextFile("config.json",{
            dir: BaseDirectory.App
        }).then((data) => {
            setConfig(JSON.parse(data));
            setProjectsDir(JSON.parse(data).projectsDir);
            readProjects(JSON.parse(data).projectsDir);
        }).catch((err) => {
            console.log(err);
            createConfig();
        });
    };

    const createConfig = async () => {
        writeTextFile("config.json",JSON.stringify({projectsDir: ""}),{
            dir: BaseDirectory.App
        }).then(() => {
            getConfig();
        }).catch((err) => {
            console.log(err);
        });
    };

    const writeConfig = async (projectsDir) => {
        writeTextFile("config.json",JSON.stringify({projectsDir: projectsDir}),{
            dir: BaseDirectory.App
        }).then(() => {
            getConfig();
        }).catch((err) => {
            console.log(err);
        });
    };

    const openFolderDialog = async () => {
        await window.__TAURI__.dialog.open({
            directory: true,
            multiple: false
        }).then((path) => {
            document.getElementById("projects-folder-input").value = path;
        }).catch((err) => {
            console.log(err);
        });
    };

    const readProjects = async (projectsDir) => {
        await readDir(projectsDir).then((data) => {
            console.log(data);
            const projectsToLoad = data.length;
            let projectsLoadedCount = 0;
              data.forEach(async (project) => {
                  await readTextFile(project.path + "\\project.json").then((data) => {
                      let projectData = JSON.parse(data);
                      projectData.path = project.path;

                      projectsLoadedCount++;
                      if (projectsLoadedCount == projectsToLoad) {
                          setProjectsLoaded(true);
                      }
                      let alreadyLoaded = false;
                      projects.forEach((proj) =>{
                          if (projectData.projectName == proj.projectName) {
                              alreadyLoaded = true;
                          }
                      })
                      if (!alreadyLoaded) {
                          setProjects((projects) => [...projects, projectData]);
                      }
                  }).catch((err) => {
                      console.log(err);
                  })
              });
        }).catch((err) => {
          console.log(err);
        })
    };

    const createNewProject = async (projectName) => {
        await createDir(projectsDir+"\\"+projectName).then(async () => {
            await writeTextFile(projectsDir+"\\"+projectName+"\\project.json",JSON.stringify({
            projectName: projectName,
            lastEdit: new Date().toLocaleString().split(",")[0],
            })).then(() => {
                readProjects(projectsDir);
            }).catch((err) => {
                console.log(err);
            });
        }).catch((err) => {
            console.log(err);
        });
    };

    const openEngine = async (projectPath) => {
        //const enginePath = "C:\\Users\\jjpgn\\Desktop\\groupproject\\Project\\COMP5530M-Group-Project\\out\\build\\x64-debug\\BuildSys.exe";
        const command = new Command("engine",[projectPath]);
        command.on('close', data => {
            console.log(`command finished with code ${data.code} and signal ${data.signal}`)
        });
        command.on('error', error => console.error(`command error: "${error}"`));
        command.stdout.on('data', line => console.log(`command stdout: "${line}"`));
        command.stderr.on('data', line => console.log(`command stderr: "${line}"`));

        const child = await command.spawn();
        console.log('pid:', child.pid);
    }

    useEffect(() => {
        if (!beencalled) {
            getConfig();
            beencalled = true;
        }
    }, []);

  return (
    <div className="container">
      {config === null && (
        <div className="loading">
          <div className="loading-spinner"></div>
          <div className="loading-text">Loading Config {BaseDirectory.App} ...</div>
        </div>
      ) || projectsDir === null && (
        <div className="loading">
          <div className="loading-spinner"></div>
          <div className="loading-text">Loading Projects...</div>
        </div>
      ) || projectsDir === "" && (
        <div className="set-config-screen">
          <div className="set-config-screen-content">
            <div className="set-config-screen-text">Set Projects Directory</div>
            <div className="set-config-screen-input">
              <input type="text" id="projects-folder-input" /><button onClick={() => openFolderDialog()}>Browse</button>
            </div>
            <div className="set-config-screen-button">
             <button onClick={() => writeConfig(document.getElementById("projects-folder-input").value)}>Save</button>
            </div>
          </div>
        </div>
      ) || projectsLoaded === false && (
        <div className="loading">
          <div className="loading-spinner"></div>
          <div className="loading-text">Loading Projects...</div>
        </div>
      ) || (
        <div className="projects-screen">
          {newProjectModalOpen && (
            <div className="new-project-modal">
              <div className="new-project-modal-content">
                <div className="new-project-modal-text">New Project</div>
                <div className="new-project-modal-input">
                  <input type="text" id="new-project-name-input" placeholder="Project Name" />
                </div>
                <div className="new-project-modal-button">
                  <button onClick={() => setNewProjectModalOpen(false)}>Cancel</button>
                  <button onClick={() => {createNewProject(document.getElementById("new-project-name-input").value); setNewProjectModalOpen(false);}}>Create</button>
                </div>
              </div>
            </div>
          )}
          <div className="new-project-button">
            <button onClick={() => setNewProjectModalOpen(true)}>New Project</button>
          </div>
          <div className="projects-list">
            {projects.map((project, key) => (
                <div key={key} className="project-item" onClick={() => openEngine(project.path) }>
                <div className="project-info">
                  <div className="project-name">{project.projectName}</div>
                  <div className="project-path">{project.path}</div>
                </div>
                <div className="project-last-edited">Edited {project.lastEdit}</div>
              </div>
            ))}
            {projects.length === 0 && (
              <div className="no-projects">
                <div className="no-projects-text">No Projects Found</div>
              </div>
            )}
          </div>
        </div>
      )}
    </div>
  );
}

export default App;
