       // ------------------------------------------------------------- *
       // noxDB - Not only XML. JSON, SQL and XML made easy for RPG

       // Company . . . : System & Method A/S - Sitemule
       // Design  . . . : Niels Liisberg

       // Unless required by applicable law or agreed to in writing, software
       // distributed under the License is distributed on an "AS IS" BASIS,
       // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.

       // Look at the header source file "QRPGLESRC" member "NOXDB"
       // for a complete description of the functionality

       // When using noxDB you need two things:
       //  A: Bind you program with "NOXDB" Bind directory
       //  B: Include the noxDB prototypes from QRPGLSERC member NOXDB

       // ------------------------------------------------------------- *
       // Using SQL cursors. Row by row is one solution

       // ------------------------------------------------------------- *
       Ctl-Opt BndDir('NOXDB') dftactgrp(*NO) ACTGRP('QILE');
      /include qrpgleRef,noxdb
       Dcl-S pRow               Pointer;
       Dcl-S sql                VarChar(512);



        // return one object with one row
          sql   = ('-
               Select *                  -
                     from qgpl/varchar         -
                     fetch first 1 row only    -
                  ');

                pRow  = json_sqlResultRow(sql);

                // Produce a JSON stream file in the root of the IFS
                json_writeJsonStmf(pRow   :
                   '/noxdb/json/one-object.json' : 1208 : *ON
                );

                // Cleanup: delete the object and disconnect
                json_delete (pRow);
                json_sqlDisconnect();

                // That's it..
                *inlr = *on;
