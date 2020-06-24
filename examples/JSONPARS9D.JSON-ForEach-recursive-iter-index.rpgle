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

       // Parse Strings

       // ------------------------------------------------------------- *
       Ctl-Opt BndDir('NOXDB') dftactgrp(*NO) ACTGRP('QILE');
      /include qrpgleRef,noxdb
       Dcl-S pJson              Pointer;
       Dcl-S msg                VarChar(50);
       Dcl-S v                  VarChar(50);
       Dcl-S i                      Int(10:0);
       Dcl-S maxid                  Int(10:0);
       Dcl-S id                     Int(10:0);
       Dcl-S index              Pointer dim(100);
       Dcl-DS list                     likeds(json_iterator);
          pJson = json_ParseFile (
                '/www/portfolio/Public/TwiSite/xpd/folder.xpd'
            );

          If json_Error(pJson) ;
             msg = json_Message(pJson);
             json_dump(pJson);
             json_delete(pJson);
             Return;
          EndIf;

          list = json_setRecursiveIterator(pJson);
          DoW json_ForEach(list);
             If ('id' = json_getName (list.this));
                id = json_getNum     (list.this);
                index(id) = json_getParent(list.this);
                If maxid < id;
                   maxid = id;
                EndIf;
             EndIf;
          EndDo;

          // Lets play with it:
          For i = 1 to maxid;
             v = json_getStr    (index(i):'title');
             Dsply v;
          EndFor;

          json_delete(pJson);
          *inlr = *on;
